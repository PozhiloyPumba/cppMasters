#ifndef __INTERPRETER_HPP__
#define __INTERPRETER_HPP__

#include <string>
#include <memory>
#include <vector>
#include <array>
#include <stack>
#include <unordered_map>
#include "lexer.hpp"
#include "genEnum.hpp"

namespace CRAM {

#define ENUM_TYPES(c) \
	c(ARRAY_WITH_BASE) \
	c(ARRAY_NO_BASE) \
	c(VAR) \
	c(NUMBER) \
	c(ADD) \
	c(SUB) \
	c(ASSIGN) \
	c(PRINT) \
	c(INPUT) \
	c(EMPTY) \
	c(ERROR)

class Interpreter final {
	std::unique_ptr<Lexer> l_;
	std::unordered_map<std::string, int> vars_{};

	std::vector<int> &memory;
	using Addr_t = size_t;
	using IterType = std::vector<std::unique_ptr<CRAM::Token>>::const_iterator;
	IterType curToken;
	size_t sizeOfInput_;

	CREATE_ENUM(NodeType, ENUM_TYPES);

	struct Node {
		NodeType type_;
		std::vector<Node *> children;
		Node *parent;
		Node(NodeType type):type_(type){}
	};
	struct VarNode: public Node {
		std::string name;
		VarNode(std::string n):Node(NodeType::VAR), name(n){}
	};
	struct NumberNode: public Node {
		int value;
		NumberNode(int val):Node(NodeType::NUMBER), value(val){}
	};

	std::vector<Node *> garbage_;

	inline NumberNode *allocNumberNode(const int val) {
		auto *node = new NumberNode(val);
		garbage_.push_back(node);
		return node;
	}
	inline VarNode *allocVarNode(const std::string &name) {
		auto *node = new VarNode(name);
		garbage_.push_back(node);
		return node;
	}
	inline Node *allocNode(const NodeType type) {
		auto *node = new Node(type);
		garbage_.push_back(node);
		return node;
	}

	Node *parseLine(const std::vector<std::unique_ptr<CRAM::Token>> &statement);
	Node *parseStatement();
	Node *parseArithm();
	Node *parseExpr();
	Node *parseTerm();
	Node *parseName();

	Node errorNode = Node(NodeType::ERROR);

	std::stack<int> evalStack_{};
	bool eval(Node *root);
	bool checkErrors(Node *root) const;
	void evalStatement(Node *curNode);

	void dump(std::ostream &out, Node *root) const;
	void PrintNodeIntoGraphviz(Node *curNode, std::ostream &out) const;
	void BuildConnectionsInGraphviz(Node *curNode, std::ostream &out) const;

public:
	Interpreter(const std::string &filename, std::vector<int> &mem);
	Interpreter(const Interpreter &) = delete;
	Interpreter operator=(const Interpreter &) = delete;
	Interpreter(Interpreter &&) = delete;
	Interpreter operator=(Interpreter &&) = delete;
	~Interpreter();
	void run();
};

}

#endif