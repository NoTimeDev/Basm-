#pragma once 

#include <cstdint>
#include <ostream>
#include <string>
#include <vector>
enum class TokenKind{
	ID, 
	NUMBER, 

	SUB,
	OPEN_BRACE,
	CLOSE_BRACE, 
	COMMA, 

	REGISTER, 
	
	MOV, 
	ADD, 


	TERMINATOR, 
	EOF_,
	NULL_,
}; 

struct Token{
	TokenKind Kind;
	uint64_t Line; 
	std::string Value; 
};

void Lex(std::vector<Token>& Tokens, std::string& Contents);
std::ostream& operator<<(std::ostream& os, Token& tk);
