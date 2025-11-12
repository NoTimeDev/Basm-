#include "./Ast.hpp"
#include <cstddef>
#include <cstdint>
#include <iostream>

Stmt::Stmt(){};
Expr::Expr(){};

void Stmt::Print(int indent){};
void Expr::Print(int indent){};

NullStmt::NullStmt(){this->Kind = NodeKind::NULL_;};
NullExpr::NullExpr(){this->Kind = NodeKind::NULL_;};

void NullStmt::Print(int indent){
	std::cout << std::string(indent*2, ' ') << "Kind : NullStmt\n";
}

void NullExpr::Print(int indent){
	std::cout << std::string(indent*2, ' ') << "Kind : NullExpr\n";
}

BlockStmt::BlockStmt(uint64_t Line, std::string Name, std::vector<std::shared_ptr<Stmt>> Body){
	this->Name  = Name;
	this->Body = Body;
	this->Kind = NodeKind::BLOCK;
	this->Line = Line;
};


void BlockStmt::Print(int indent){
	std::cout << std::string(indent*2, ' ') << "Kind : BlockStmt\n";
	std::cout << std::string(indent*2, ' ') << "Name : " << this->Name << "\n";
	std::cout << std::string(indent*2, ' ') << "Body : \n";
	++indent;
	for(auto& i : this->Body){
		i->Print(indent);
	}
	--indent;
}

InstructionStmt::InstructionStmt(NodeKind Kind, uint64_t Line, std::shared_ptr<Expr> Op1, std::shared_ptr<Expr> Op2, uint8_t Size){
	this->Op1 = Op1;
	this->Op2 = Op2;
	this->Size = Size; 
	this->Kind = Kind;
	this->Line = Line;
};

void InstructionStmt::Print(int indent){
	std::cout << std::string(indent*2, ' ') << "Kind : InstructionStmt(" << (int)this->Kind << ")\n";
	std::cout << std::string(indent*2, ' ') << "Size : " << (int)this->Size << "\n";
	std::cout << std::string(indent*2, ' ') << "Op1 : \n";
	this->Op1->Print(indent+1);
	std::cout << std::string(indent*2, ' ') << "Op2 : \n";
	this->Op2->Print(indent+1);
}

NumberExpr::NumberExpr(uint64_t Line, uint64_t Value){
	this->Size = 0; 
	this->Value = Value;
	this->Kind = NodeKind::NUMBER;
	this->Line = Line;
}

void NumberExpr::Print(int indent){
	std::cout << std::string(indent*2, ' ') << "Kind : NumberExpr\n";
	std::cout << std::string(indent*2, ' ') << "Size : ?\n";
	std::cout << std::string(indent*2, ' ') << "Value : " << this->Value << "\n";

}

RegisterExpr::RegisterExpr(uint64_t Line, uint8_t Id, uint8_t Size){
	this->Id = Id;
	this->Size = Size; 
	this->Kind = NodeKind::REGISTER;
	this->Line = Line;
}

void RegisterExpr::Print(int indent){
	std::cout << std::string(indent*2, ' ') << "Kind : RegisterExpr\n";
	std::cout << std::string(indent*2, ' ') << "Size : " << (int)this->Size << "\n";
	std::cout << std::string(indent*2, ' ') << "Id : " << (int)this->Id << "\n";
}

