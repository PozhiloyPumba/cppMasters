#ifndef __TOKEN_HPP__
#define __TOKEN_HPP__

#include "lexem.hpp"
#include <string>

namespace PM {

class Token {
protected:
	Lexem type_;
public:
	Token(Lexem type): type_(type) {}

	inline Lexem getType() const { return type_; }
};

class Var final : public Token {
	std::string name_;

public:
	Var(const std::string &name): Token(Lexem::VAR), name_(name){}

	inline const std::string &getName() const { return name_; }
};


class Number final : public Token {
	int value_;

public:
	Number(const int val): Token(Lexem::NUMBER), value_(val){}

	inline int getValue() const { return value_; }
};

}

#endif