#include "Lexer.hpp"
#include <cstdint>
#include <iostream>
#include <unordered_map>


void Add_Sized_Keyword(std::string Word, TokenKind Kind, std::unordered_map<std::string, TokenKind>& als){
	als[Word + "q"] = Kind; 
	als[Word + "d"] = Kind; 
	als[Word + "w"] = Kind; 
	als[Word + "b"] = Kind; 
}

void Add_Register(int reg,  std::unordered_map<std::string, TokenKind>& als){
	for(int i = 0; i < reg; i++){
		als["r" + std::to_string(i)] = TokenKind::REGISTER;
		als["r" + std::to_string(i) + "d"] = TokenKind::REGISTER;
		als["r" + std::to_string(i) + "w"] = TokenKind::REGISTER;
		als["r" + std::to_string(i) + "b"] = TokenKind::REGISTER;
	}
}

void Lex(std::vector<Token>& Tokens, std::string& Contents){
	uint64_t Pos = 0; 
	uint64_t Line = 1; 
	bool Err = false;
	 
	std::unordered_map<std::string, TokenKind> Keywords = {
	};
	Add_Sized_Keyword("mov", TokenKind::MOV, Keywords);
	Add_Sized_Keyword("add", TokenKind::ADD, Keywords);
	Add_Register(15, Keywords);

	while(Contents.size() > Pos){
		switch(Contents[Pos]){
			case '{':
				Tokens.push_back({TokenKind::OPEN_BRACE, Line, "{"});
				++Pos; 
				break;
			case '}':
				Tokens.push_back({TokenKind::CLOSE_BRACE, Line, "}"});
				++Pos;
				break;
			case ',':
				Tokens.push_back({TokenKind::COMMA, Line , ","});
				++Pos; 
				break; 
			case ';':
				Tokens.push_back({TokenKind::TERMINATOR, Line, ";"});
				++Pos; break;
			case '-':
				Tokens.push_back({TokenKind::SUB, Line, "-"});
				++Pos; break;
			case '\n':
				Tokens.push_back({TokenKind::TERMINATOR, Line, "newline"});
				++Line; ++Pos;
				break; 
			case ' ':
			case '\t':
			case '\r':
				++Pos; break;
			default:
				if(std::isdigit(Contents[Pos])){
					std::string Number;
					while(std::isdigit(Contents[Pos])){
						Number+=Contents[Pos];
						++Pos;
					}
					Tokens.push_back({TokenKind::NUMBER, Line, Number});
					break;
				}else if(Contents[Pos] == '.' || Contents[Pos] == '_' || std::isalpha(Contents[Pos])){
					std::string Word;
					while(Contents[Pos] == '.' || Contents[Pos] == '_' || std::isalnum(Contents[Pos])){
						Word+=Contents[Pos];
						++Pos;
					}
					if(Keywords.find(Word) == Keywords.end()){
						Tokens.push_back({TokenKind::ID, Line, Word});
					}else{
						Tokens.push_back({Keywords[Word], Line, Word});
					}
					break;
				}
				std::cerr << "basm:" << Line << ": Unkown token '" << Contents[Pos] << "'\n";
				Err = true;
				break;
		}
	}
	if(Err){
		exit(1);
	}
	Tokens.push_back({TokenKind::EOF_, Tokens.back().Line, "EOF"});
}

std::ostream& operator<<(std::ostream& os, Token& tk){
	os << "{ \"Kind\" : " << static_cast<int>(tk.Kind) << ", \"Line\" : " << tk.Line << ", \"Value\" : \"" << tk.Value << "\" }";
	return os;
}
