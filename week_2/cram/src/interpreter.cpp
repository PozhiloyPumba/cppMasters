#include "interpreter.hpp"
#include <fstream>
#include <algorithm>

namespace CRAM {

Interpreter::Interpreter(const std::string &filename, std::vector<int> &mem): memory(mem) {
	std::ifstream f(filename);
	if(!f)
        throw std::runtime_error("file not opened: " + filename);

	l_ = std::make_unique<Lexer>(f);
	l_->read();
	f.close();
	sizeOfInput_ = l_->getStatementsCount();
}

Interpreter::~Interpreter() {
	std::for_each(garbage_.begin(), garbage_.end(), [](auto *n){ delete n;});
}

#define ERROR_HANDLE(node) \
	if((node)->type_ == NodeType::ERROR) { \
		return &errorNode; \
	}

Interpreter::Node *Interpreter::parseStatement() {
	auto *arithm = parseArithm();

	ERROR_HANDLE(arithm);

	if((*curToken)->getType() == Lexem::SEMICOLON) {
		return arithm;
	}

	if((*curToken)->getType() != Lexem::ASSIGN) {
		return &errorNode;
	}
	++curToken;
	auto *statement = parseStatement();
	if((*curToken)->getType() != Lexem::SEMICOLON) {
		return &errorNode;
	}

	auto *assign = allocNode(NodeType::ASSIGN);
	assign->children.push_back(arithm);
	assign->children.push_back(statement);
	arithm->parent = assign;
	statement->parent = assign;
	return assign;
}

Interpreter::Node *Interpreter::parseArithm() {
	auto *tryExpr = parseExpr();

	ERROR_HANDLE(tryExpr);
	if(tryExpr->type_ == NodeType::EMPTY) {
		return &errorNode;
	}

	auto tokenType = (*curToken)->getType();
	switch (tokenType) {
		case Lexem::ADD:
		case Lexem::SUB: {
			auto convert = [](auto t) {
				switch(t) {
					case Lexem::ADD: return NodeType::ADD;
					case Lexem::SUB: return NodeType::SUB;
					default:;
				}
				return NodeType::ERROR;
			};
			++curToken;

			if (((*curToken)->getType() == Lexem::ADD) ||
				((*curToken)->getType() == Lexem::SUB)) {
				return &errorNode;
			}

			auto *arithm = parseArithm();
			ERROR_HANDLE(arithm);

			auto *op = allocNode(convert(tokenType));
			op->children.push_back(tryExpr);
			op->children.push_back(arithm);
			arithm->parent = op;
			tryExpr->parent = op;
			return op;
		}
		default:;
	}
	return tryExpr;
}


Interpreter::Node *Interpreter::parseExpr() {
	auto savedCur = curToken;
	auto tokenType = (*curToken)->getType();
	switch(tokenType) {
		case Lexem::VAR: {
			++curToken;
			if ((*curToken)->getType() != Lexem::OP_BRACK) {
				curToken = savedCur;
				return parseTerm();
			}
			++curToken;
			auto *arithm = parseArithm();
			ERROR_HANDLE(arithm);
			if(arithm->type_ == NodeType::EMPTY) {
				return &errorNode;
			}
			if ((*curToken)->getType() == Lexem::EMPTY) {
				++curToken;
			}
			if ((*curToken)->getType() != Lexem::CL_BRACK) {
				return &errorNode;
			}
			auto *arr = allocNode(NodeType::ARRAY_WITH_BASE);
			std::swap(curToken, savedCur);
			auto *name = parseName();
			std::swap(curToken, savedCur);
			arr->children.push_back(name);
			arr->children.push_back(arithm);
			arithm->parent = arr;
			name->parent = arr;
			++curToken;
			return arr;
		}
		case Lexem::OP_BRACK: {
			// [[ -> c[..] [c...]
			++curToken;
			if ((*curToken)->getType() == Lexem::OP_BRACK) {
				++curToken;
				auto *arithm = parseArithm();
				if(arithm->type_ == NodeType::ERROR) {
					curToken = savedCur + 1;
					arithm = parseArithm();
					ERROR_HANDLE(arithm);
					if ((*curToken)->getType() != Lexem::CL_BRACK) {
						return &errorNode;
					}
					if(arithm->type_ == NodeType::EMPTY) {
						return &errorNode;
					}

					auto *arr = allocNode(NodeType::ARRAY_NO_BASE);
					arr->children.push_back(arithm);
					arithm->parent = arr;
					++curToken;
					return arr;
				}
				if ((*curToken)->getType() != Lexem::CL_BRACK) {
					return &errorNode;
				}
				auto *arr = allocNode(NodeType::ARRAY_WITH_BASE);
				auto *name = allocVarNode("c");
				arr->children.push_back(name);
				arr->children.push_back(arithm);
				arithm->parent = arr;
				name->parent = arr;
				++curToken;
				return arr;
			}
			curToken = savedCur;

			++curToken;
			auto *arithm = parseArithm();
			if(arithm->type_ == NodeType::ERROR) {
				curToken = savedCur;
				return parseTerm();
			}
			if(arithm->type_ == NodeType::EMPTY) {
				curToken = savedCur;
				return parseTerm();
			}
			if ((*curToken)->getType() != Lexem::CL_BRACK) {
				return &errorNode;
			}
			auto *arr = allocNode(NodeType::ARRAY_NO_BASE);
			arr->children.push_back(arithm);
			arithm->parent = arr;
			++curToken;
			return arr;
		}
		case Lexem::EMPTY:
		default:;
	}
	return parseTerm();
}

Interpreter::Node *Interpreter::parseTerm() {
	auto savedCur = curToken;
	auto tokenType = (*curToken)->getType();
	switch(tokenType) {
		case Lexem::EMPTY: {
			++curToken;
			return allocNode(NodeType::EMPTY);
		}
		case Lexem::NUMBER: {
			++curToken;
			return allocNumberNode(static_cast<const Number *>(savedCur->get())->getValue());
		}
		case Lexem::ADD:
		case Lexem::SUB: {
			++curToken;
			if (((*curToken)->getType() == Lexem::ADD) ||
				((*curToken)->getType() == Lexem::SUB)) {
				return &errorNode;
			}

			auto *term = parseTerm();
			ERROR_HANDLE(term);
			auto convert = [](auto t) {
				switch(t) {
					case Lexem::ADD: return NodeType::ADD;
					case Lexem::SUB: return NodeType::SUB;
					default:;
				}
				return NodeType::ERROR;
			};
			auto *op = allocNode(convert(tokenType));
			op->children.push_back(term);
			term->parent = op;
			return op;
		}
		default:;
	}
	return parseName();
}

Interpreter::Node *Interpreter::parseName() {
	auto savedCur = curToken;
	auto tokenType = (*curToken)->getType();
	switch(tokenType) {
		case Lexem::VAR: {
			++curToken;
			return allocVarNode(static_cast<const Var *>(savedCur->get())->getName());
		}
		case Lexem::OP_BRACK: {
			++curToken;
			return allocVarNode("c");
		}
		case Lexem::CL_BRACK: {
			++curToken;
			if ((*curToken)->getType() != Lexem::OP_BRACK)
				return &errorNode;
			++curToken;
			return allocVarNode("x");
		}
		default:;
	}
	return &errorNode;
}

Interpreter::Node *Interpreter::parseLine(const std::vector<std::unique_ptr<CRAM::Token>> &statement) {
	curToken = statement.cbegin();
	Node *val = nullptr;
	Node *op = nullptr;
	switch(statement[0]->getType()) {
		case Lexem::INPUT:
		case Lexem::PRINT: {
			auto convert = [](auto t) {
				switch(t) {
					case Lexem::INPUT: return NodeType::INPUT;
					case Lexem::PRINT: return NodeType::PRINT;
					default:;
				}
				return NodeType::ERROR;
			};
			++curToken;
			val = parseStatement();
			op = allocNode(convert(statement[0]->getType()));
			op->children.push_back(val);
			val->parent = op;
			val = op;
			break;
		}
		default:
			val = parseStatement();
	}
	return val;
}

bool Interpreter::checkErrors(Node *root) const {
	if(root == &errorNode) {
		return true;
	}
	for (auto it = root->children.begin(); it != root->children.end(); ++it) {
		Node *curChild = *it;
		if(checkErrors(curChild))
			return true;
	}
	return false;
}

bool Interpreter::eval(Node *root) {
	#ifndef NDEBUG
	dump(std::cout, root);
	#endif

	if(checkErrors(root)) {
		return false;
	}

	evalStatement(root);
	return true;
}

void Interpreter::evalStatement(Node *curNode) {
	auto popt = [this]() {
		int val = evalStack_.top();
		evalStack_.pop();
		return val;
	};
	auto isArr = [](auto node) {
		return (node->type_ == NodeType::ARRAY_NO_BASE) || 
				(node->type_ == NodeType::ARRAY_WITH_BASE);
	};

	switch(curNode->type_) {
		case NodeType::ARRAY_WITH_BASE: {
			evalStatement(curNode->children[0]); // base
			evalStatement(curNode->children[1]); // off
			int off = popt();
			int base = popt();
			if(isArr(curNode->children[1])) {
				off = memory[off];
			}
			evalStack_.push(base+off);
			break;
		}
		case NodeType::ARRAY_NO_BASE: {
			evalStatement(curNode->children[0]); // off
			int off = popt();
			if(isArr(curNode->children[0])) {
				off = memory[off];
			}
			evalStack_.push(off);
			break;
		}
		case NodeType::VAR: {
			evalStack_.push(vars_[static_cast<const VarNode *>(curNode)->name]);
			break;
		}
		case NodeType::NUMBER: {
			evalStack_.push(static_cast<const NumberNode *>(curNode)->value);
			break;
		}
		case NodeType::ADD: {
			evalStatement(curNode->children[0]);
			int res = popt();
			if(isArr(curNode->children[0])) {
				res = memory[res];
			}
			if(curNode->children.size() == 2) {
				evalStatement(curNode->children[1]);
				int res1 = popt();
				if(isArr(curNode->children[1])) {
					res1 = memory[res1];
				}
				res += res1;
			}
			evalStack_.push(res);
			break;
		}
		case NodeType::SUB: {
			evalStatement(curNode->children[0]);
			int res = popt();
			if(isArr(curNode->children[0])) {
				res = memory[res];
			}
			res = -res;
			if(curNode->children.size() == 2) {
				evalStatement(curNode->children[1]);
				int res1 = popt();
				if(isArr(curNode->children[1])) {
					res1 = memory[res1];
				}
				res += res1;
				res = -res;
			}
			evalStack_.push(res);
			break;
		}
		case NodeType::ASSIGN: {
			evalStatement(curNode->children[1]);
			int res = popt();
			if(isArr(curNode->children[1])) {
				res = memory[res];
			}
			if(curNode->children[0]->type_ == NodeType::VAR) {
				vars_[static_cast<const VarNode *>(curNode->children[0])->name] = res;
				evalStack_.push(res);
				break;
			}
			evalStatement(curNode->children[0]);
			memory[popt()] = res;
			evalStack_.push(res);
			break;
		}
		case NodeType::PRINT: {
			evalStatement(curNode->children[0]);
			if(!isArr(curNode->children[0])) {
				std::cout << popt() << std::endl;
				break;
			}
			std::cout << memory[popt()] << std::endl;
			break;
		}
		case NodeType::INPUT: {
			if(!isArr(curNode->children[0])) {
				std::cin >> vars_[static_cast<const VarNode *>(curNode->children[0])->name];
				break;
			}
			evalStatement(curNode->children[0]);
			std::cin >> memory[popt()];
			break;
		}
		default:;
	}
}


void Interpreter::run() {
	size_t curPos = 0;

	for(; curPos < sizeOfInput_; ++curPos) {
		auto &curStatement = l_->get_statement(curPos);
		auto *root = parseLine(curStatement);
		if(!eval(root)) {
			std::cerr << "Error in " << curPos << " statement" << std::endl;
			return;
		}
	}
}

void Interpreter::dump(std::ostream &out, Node *root) const {
	if (!root)
		return;

	out << "digraph tree {\n"
			"rankdir = \"LR\"\n"
			"node [fontsize=10, shape=box, height=0.5]\n"
			"edge [fontsize=10]\n";

	PrintNodeIntoGraphviz(root, out);
	BuildConnectionsInGraphviz(root, out);

	out << "}\n";
}

void Interpreter::PrintNodeIntoGraphviz(Node *curNode, std::ostream &out) const {
	out << "\"" << curNode << "\" [label = \"";
	out << toString(curNode->type_);
	if (curNode->type_ == NodeType::VAR){
		out << " " << static_cast<const VarNode *>(curNode)->name;
	}
	else if (curNode->type_ == NodeType::NUMBER) {
		out << " " << static_cast<const NumberNode *>(curNode)->value;
	}
	out << "\"]\n";

	for (auto it = curNode->children.begin(); it != curNode->children.end(); ++it) {
		Node *curChild = *it;
		if (curChild)
			PrintNodeIntoGraphviz(curChild, out);
	}
}

void Interpreter::BuildConnectionsInGraphviz(Node *curNode, std::ostream &out) const {
	for (auto it = curNode->children.begin(); it != curNode->children.end(); ++it) {
		Node *curChild = *it;
		if (curChild)
			out << "\"" << curNode << "\" -> \"" << curChild << "\"\n";
	}

	for (auto it = curNode->children.begin(); it != curNode->children.end(); ++it) {
		Node *curChild = *it;
		if (curChild)
			BuildConnectionsInGraphviz (curChild, out);
	}
}
}