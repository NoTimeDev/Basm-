#pragma once 

#include <cstdint>
#include <memory>
#include <vector>
#include "../Lexer/Lexer.hpp"
#include "Ast.hpp"

class Parser{
	std::vector<Token>& Tokens;
	uint64_t Pos = 0;
	bool Err = false;
	uint8_t MaxReg = 0;
public:
	Parser(std::vector<Token>& Tokens); 
	std::shared_ptr<Stmt> ParseStmt();
	std::shared_ptr<Expr> ParseExpr();

	Token CToken();
	TokenKind CTokenKind();
	Token Advance();
	Token Expect(TokenKind Kind, const char* Err);
	void ExpectTerminator(const char* after);

	ProgramNode Parse();
	void Recover();
};
