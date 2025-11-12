#include "Parser.hpp"
#include "Ast.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <iostream>


std::shared_ptr<Stmt> NULLSTMT = std::make_shared<NullStmt>();
std::shared_ptr<Expr> NULLEXPR = std::make_shared<NullExpr>();

#define CheckOperand(op) if(op->Kind == NodeKind::NULL_){this->Recover(); return NULLSTMT;}
#define CheckToken(tk) if(tk.Kind == TokenKind::NULL_){this->Recover(); return NULLSTMT;}

#define CheckOperandE(op) if(op->Kind == NodeKind::NULL_){this->Recover(); return NULLEXPR;}
#define CheckTokenE(tk) if(tk.Kind == TokenKind::NULL_){this->Recover(); return NULLEXPR;}

Parser::Parser(std::vector<Token>& Tokens) : Tokens(Tokens){};

Token Parser::CToken(){
	return this->Tokens[this->Pos];
}

TokenKind Parser::CTokenKind(){
	return this->Tokens[this->Pos].Kind;
}

Token Parser::Advance(){
	Token Ctk = this->CToken();
	if(Ctk.Kind == TokenKind::EOF_){
		std::cerr << "basm:" << Ctk.Line << ":error: " << "Unexpected eof\n";
		exit(1);
	}
	this->Pos++;
	return Ctk;
}

Token Parser::Expect(TokenKind Kind, const char* Err){
	Token Ctk = this->Advance();
	if(Ctk.Kind != Kind){
		std::cerr << "basm:" << Ctk.Line << ":error: Expected" << Err << "But recived " << Ctk.Value << "\n";
		this->Err = true;
		return Token{.Kind = TokenKind::NULL_, .Line = 0, .Value = ""};
	}
	return Ctk;
}

void Parser::Recover(){
	while(this->CTokenKind() != TokenKind::TERMINATOR){
		if(this->CTokenKind() == TokenKind::EOF_){
			exit(1);
		}
		this->Pos++;
	}
}

void Parser::ExpectTerminator(const char* after){
	Token tk = this->Advance();
	if(tk.Kind != TokenKind::TERMINATOR){
		std::cerr << "basm:" << tk.Line << ":error: Expect newline or semicolon after " << after << "\n";
		this->Err = true;
		this->Recover();
	}
}

ProgramNode Parser::Parse(){
	std::vector<std::shared_ptr<Stmt>> Body;
	while(Pos < this->Tokens.size() && this->CTokenKind() != TokenKind::EOF_){
		if(this->CTokenKind() == TokenKind::TERMINATOR) this->Advance();
		else Body.push_back(this->ParseStmt());
	}
	if(this->Err){
		exit(1);
	}
	return {.Body = Body, .MaxReg = this->MaxReg};
}


std::shared_ptr<Stmt> Parser::ParseStmt(){
	TokenKind Kind = this->CTokenKind();
	switch(this->CTokenKind()){
		case TokenKind::ID:{
			Token Name = this->Advance();
			if(this->CTokenKind() != TokenKind::OPEN_BRACE){
				std::cerr << "basm:" << Name.Line  << ":error: Expected '{' following a identifier\n"; 
				this->Recover();
				this->Err = true;
				return NULLSTMT;
			}
			this->Advance();
			std::vector<std::shared_ptr<Stmt>> Body;
			while(this->CTokenKind() != TokenKind::CLOSE_BRACE){
				if(this->CTokenKind() == TokenKind::TERMINATOR) this->Advance();
				else Body.push_back(this->ParseStmt());
			}
			this->Expect(TokenKind::CLOSE_BRACE, "}");
			return std::make_shared<BlockStmt>(Name.Line, Name.Value,  Body);
		}
		case TokenKind::MOV:
		case TokenKind::ADD:{
			uint8_t Size = 0;
			Token tk = this->Advance();
			switch(tk.Value[3]){
				case 'q': Size = 8; break; 
				case 'd': Size = 4; break; 
				case 'w': Size = 2; break; 
				case 'b': Size = 1; break; 
			};
			std::shared_ptr<Expr> Op1 = this->ParseExpr();
			CheckOperand(Op1);
			CheckToken(this->Expect(TokenKind::COMMA, "','"));
			std::shared_ptr<Expr> Op2 = this->ParseExpr();
			CheckOperand(Op2);
			this->ExpectTerminator("operand");
			return std::make_shared<InstructionStmt>(
				Kind == TokenKind::ADD ? NodeKind::ADD : NodeKind::MOV,
																						tk.Line, Op1, Op2, Size);
		}
		default:
			std::cerr << "basm:" << this->CToken().Line  << ":error: No stmt for tokenkind " << static_cast<int>(this->Advance().Kind) << "\n"; 
			this->Err = true;
			break;
	}
	return NULLSTMT; 
}

std::shared_ptr<Expr> Parser::ParseExpr(){
	switch(this->CTokenKind()){
		case TokenKind::SUB:{
			Token tk = this->Advance();
			Token Num = this->Expect(TokenKind::NUMBER, "int"); 
			CheckTokenE(Num);
			int64_t Number = std::stoll("-" + Num.Value);
			return std::make_shared<NumberExpr>(tk.Line,  *(reinterpret_cast<uint64_t*>(&Number)));
		}
		case TokenKind::NUMBER:{
			Token Num = this->Advance();
			return std::make_shared<NumberExpr>(Num.Line, std::stoull(Num.Value));
		}
		case TokenKind::REGISTER:{
			Token Reg = this->Advance();
			uint8_t Size = 0;
			switch(Reg.Value.back()){
				case 'd': Size = 4; break; 
				case 'w': Size = 2; break; 
				case 'b': Size = 1; break; 
				default:
					Size = 8;
					break;
			};
			std::string Num;
			for(auto& i : Reg.Value) if(std::isdigit(i)) Num+=i;
			uint8_t Id = std::stoi(Num);
			if(Id > this->MaxReg) this->MaxReg = Id;
			return std::make_shared<RegisterExpr>(Reg.Line, Id, Size);
		}
		default:
			std::cerr << "basm:" << this->CToken().Line  << ":error: No expr for tokenkind " << static_cast<int>(this->Advance().Kind) << "\n"; 
			this->Err = true;
			break;
	}
	return NULLEXPR; 
}
