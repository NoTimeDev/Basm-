#include "CodeGen.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <memory>
#include <iostream>

#define BOF_VERSION 1
BfCode::BfCode(){

}

void BfCode::Add(uint64_t count, bool Term){
	for(uint64_t i = 0; i < count; ++i) this->Code.push_back('+');
	if(Term) this->Code.push_back('\n');

}
void BfCode::Sub(uint64_t count, bool Term){
	for(uint64_t i = 0; i < count; ++i) this->Code.push_back('-');
	if(Term) this->Code.push_back('\n');
}
void BfCode::MoveR(uint64_t count, bool Term){
	for(uint64_t i = 0; i < count; ++i) this->Code.push_back('>');
	if(Term) this->Code.push_back('\n');
	this->Pos+=count;
}
void BfCode::MoveL(uint64_t count, bool Term){
	for(uint64_t i = 0; i < count; ++i) this->Code.push_back('<');
	if(Term) this->Code.push_back('\n');
	this->Pos-=count;
}
void BfCode::OpenLoop(bool Term){
	this->Code.push_back('[');
	if(Term) this->Code.push_back('\n');
}
void BfCode::CloseLoop(bool Term){
	this->Code.push_back(']');
	if(Term) this->Code.push_back('\n');
}
void BfCode::MoveRel(uint64_t pos, bool Term){
	if(pos > this->Pos){
		this->MoveR(pos - this->Pos, Term);
	}else	if(pos < this->Pos){
		this->MoveL(this->Pos - pos, Term);
	}
}
void BfCode::Print(bool Term){
	this->Code.push_back('.');
	if(Term) this->Code.push_back('\n');
}

CodeGen::CodeGen(ProgramNode& Body, std::unordered_map<std::string, uint64_t>& SymTable) : Body(Body.Body), Prog(Body), SymTable(SymTable){
	this->Code = BfCode();
	this->Imov = 7 * 8; 
	this->Reg = (this->Prog.MaxReg  + 1) * 8; 
	this->Spec = 2;
	this->Temp = 2 * 8;
}

void CodeGen::Copy(uint64_t Dest, uint64_t Src){
	this->Code.MoveRel(Dest);
	this->Code.OpenLoop(); this->Code.Sub(1); this->Code.CloseLoop();

	uint16_t Tmp = this->Imov + this->Reg + this->Spec;

	this->Code.MoveRel(Src);
	this->Code.OpenLoop();
	this->Code.Sub(1);
	this->Code.MoveRel(Dest);
	this->Code.Add(1);
	this->Code.MoveRel(Tmp);
	this->Code.Add(1);
	this->Code.MoveRel(Src);
	this->Code.CloseLoop(true);

	
	this->Code.MoveRel(Tmp);
	this->Code.OpenLoop();
	this->Code.Sub(1);
	this->Code.MoveRel(Src);
	this->Code.Add(1);
	this->Code.MoveRel(Tmp);
	this->Code.CloseLoop(true);
}

void CodeGen::Add(uint64_t Pos1, uint64_t Pos2){ 
	uint16_t Tmp = this->Imov + this->Reg + this->Spec + 1;
	this->Copy(Tmp, Pos2);
	this->Code.MoveRel(Tmp);
	this->Code.OpenLoop();
	this->Code.Sub(1);
	this->Code.MoveRel(Pos1);
	this->Code.Add(1);
	this->Code.MoveRel(Tmp);
	this->Code.CloseLoop(true);
}

void CodeGen::Mth(uint64_t Pos1, uint64_t Pos2, uint64_t Res){
	uint16_t Tmp1 = this->Imov + this->Reg + this->Spec + 2;
	uint16_t Tmp2 = this->Imov + this->Reg + this->Spec + 3;

	this->Copy(Tmp1, Pos1);
	this->Copy(Tmp2, Pos2);

	this->Code.MoveRel(Res);
	this->Code.OpenLoop(); this->Code.Sub(1); this->Code.CloseLoop();

	this->Code.MoveRel(Tmp1); 
	this->Code.OpenLoop();
	
	this->Code.MoveRel(Tmp2 + 1);
	this->Code.Add(1);
	
	this->Code.MoveRel(Tmp2);
	this->Code.OpenLoop();
	this->Code.MoveRel(Tmp2+1);
	this->Code.Sub(1);
	this->Code.CloseLoop();
	
	this->Code.MoveR(1);

	this->Code.Pos = Tmp2+1;
	this->Code.OpenLoop();
	this->Code.Sub(1);
	this->Code.MoveRel(Res);
	this->Code.Add(1);
	this->Code.MoveRel(Tmp2 + 2);
	this->Code.CloseLoop();

	this->Code.MoveRel(Tmp2);
	this->Code.Sub(1);
	
	this->Code.MoveRel(Tmp1); 
	this->Code.Sub(1);
	
	this->Code.MoveRel(Tmp1); 

	this->Code.CloseLoop(true);

	this->Code.MoveRel(Tmp1);
	this->Code.OpenLoop(); this->Code.Sub(1); this->Code.CloseLoop();
	
	this->Code.MoveRel(Tmp2);
	this->Code.OpenLoop(); this->Code.Sub(1); this->Code.CloseLoop();

	this->Code.MoveRel(Tmp2+1);
	this->Code.OpenLoop(); this->Code.Sub(1); this->Code.CloseLoop();

}

void CodeGen::GenStmt(std::shared_ptr<Stmt> Node){
	switch(Node->Kind){
		case NodeKind::BLOCK:{
			//rn do nothing
			std::shared_ptr<BlockStmt> Block = std::dynamic_pointer_cast<BlockStmt>(Node);
			for(auto& i : Block->Body){
				this->GenStmt(i);
			}
			break;
		}
		case NodeKind::MOV:{
			std::shared_ptr<InstructionStmt> Move = std::dynamic_pointer_cast<InstructionStmt>(Node);
			if(Move->Op1->Kind == NodeKind::REGISTER){
				std::shared_ptr<RegisterExpr> Reg = std::dynamic_pointer_cast<RegisterExpr>(Move->Op1);
				
				if(Move->Op2->Kind == NodeKind::NUMBER){
					std::shared_ptr<NumberExpr> Number = std::dynamic_pointer_cast<NumberExpr>(Move->Op2);
					for(int i = 0; i < Move->Size; ++i){
						this->Code.MoveRel(this->Imov + Reg->Id * 8 + i);
						this->Code.OpenLoop();
						this->Code.Sub(1);
						this->Code.CloseLoop(true);
						this->Code.Add((Number->Value >> (i * 8)) & 0xFF);
					}
				}else if(Move->Op2->Kind == NodeKind::REGISTER){
					std::shared_ptr<RegisterExpr> Reg2 = std::dynamic_pointer_cast<RegisterExpr>(Move->Op2);
					uint16_t Reg1Pos = this->Imov + Reg->Id * 8;	
					uint16_t Reg2Pos = this->Imov + Reg2->Id * 8;	
				
					for(int  i = 0; i < Move->Size; ++i){
						this->Copy(Reg1Pos + i, Reg2Pos + i);
					}
				}

			} 
			break; 
		}
		case NodeKind::ADD:{
			std::shared_ptr<InstructionStmt> Add = std::dynamic_pointer_cast<InstructionStmt>(Node);
			if(Add->Op1->Kind == NodeKind::REGISTER){
				std::shared_ptr<RegisterExpr> Reg = std::dynamic_pointer_cast<RegisterExpr>(Add->Op1);
				if(Add->Op2->Kind == NodeKind::REGISTER){
					std::shared_ptr<RegisterExpr> Reg2 = std::dynamic_pointer_cast<RegisterExpr>(Add->Op2);
					uint16_t Reg1Pos = this->Imov + Reg->Id * 8;	
					uint16_t Reg2Pos = this->Imov + Reg2->Id * 8;	
					int i = 0;
					uint16_t Tmp = this->Imov + this->Reg + this->Spec + 6;
					uint16_t Carry = this->Imov + this->Reg; 
					uint16_t TrueByte = this->Imov + this->Reg + 1;
				
					while(i < Add->Size - 1){
						this->Copy(Tmp, Reg1Pos + i);
						this->Add(Reg1Pos + i, Reg2Pos + i);
						this->Add(Reg1Pos + i, Carry);

						this->Code.MoveRel(Carry);
						this->Code.OpenLoop(); this->Code.Sub(1); this->Code.CloseLoop();

						this->Mth(Tmp, Reg1Pos + i, TrueByte);
						this->Code.MoveRel(TrueByte);
						this->Code.OpenLoop();
						this->Code.Sub(1);
						this->Code.MoveRel(Carry);
						this->Code.OpenLoop(); this->Code.Sub(1); this->Code.CloseLoop();
						this->Code.Add(1);
						this->Code.MoveRel(TrueByte);
						this->Code.CloseLoop(true);

						this->Mth(Reg2Pos + i, Reg1Pos + i, TrueByte);
						this->Code.MoveRel(TrueByte);
						this->Code.OpenLoop();
						this->Code.MoveRel(Carry);
						this->Code.OpenLoop(); this->Code.Sub(1); this->Code.CloseLoop();
						this->Code.Add(1);
						this->Code.MoveRel(TrueByte);
						this->Code.Sub(1);
						this->Code.CloseLoop(true);


						this->Code.MoveRel(Tmp);
						this->Code.OpenLoop(); this->Code.Sub(1); this->Code.CloseLoop();
						this->Code.MoveRel(TrueByte);
						this->Code.OpenLoop(); this->Code.Sub(1); this->Code.CloseLoop();
						++i;
					}
					this->Add(Reg1Pos + i, Reg2Pos + i);
					this->Add(Reg1Pos + i, Carry);	
					this->Code.MoveRel(Carry);
					this->Code.OpenLoop(); this->Code.Sub(1); this->Code.CloseLoop();
				}
			} 
			break; 
		}
		default: 
			break; 
	}	
}

void CodeGen::Gen(std::string Name){
	for(auto& i : this->Body){
		this->GenStmt(i);
	}

	std::ofstream File(Name);
	this->Code.Code.push_back('\n');
	File.write(this->Code.Code.data(), this->Code.Code.size());
}
