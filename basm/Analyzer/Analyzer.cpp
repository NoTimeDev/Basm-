#include "Analyzer.hpp"
#include <cstdint>
#include <memory>
#include <vector>
#include <iostream>

Analyzer::Analyzer(ProgramNode& Body) : Body(Body.Body){
	
}

void Analyzer::Analyze(){
	for(auto& Node : this->Body){
		switch (Node->Kind) {
			case NodeKind::BLOCK:{
				std::shared_ptr<BlockStmt> Block = std::dynamic_pointer_cast<BlockStmt>(Node);
				if(this->Symbols.find(Block->Name) == this->Symbols.end()){
					this->Symbols[Block->Name] = this->Id++;
					for(auto& i : Block->Body){
						this->AnalyzeStmt(i);
					}
				}else{
					std::cerr << "basm:" << Node->Line << ":error: '" << Block->Name << "' is already defined\n";
					this->Err = true; 	
				}
				break;
			}
			default:
				break;
		}
	}


	if(this->Err){
		exit(1);
	}
}

void Analyzer::AnalyzeStmt(std::shared_ptr<Stmt> Node){
	switch (Node->Kind) {
		case NodeKind::MOV:
		case NodeKind::ADD:{
			std::shared_ptr<InstructionStmt> Move = std::dynamic_pointer_cast<InstructionStmt>(Node);
			if(!this->ValidCombination(Move->Op1, Move->Op2)){
				std::cerr << "basm:" << Node->Line << ":error: unexpected combination of operands\n";
				this->Err = true; 	
			}

			if(Move->Op1->Size != 0 && Move->Op1->Size != Move->Size){
				std::cerr << "basm:" << Move->Op1->Line << ":error: unexpected operand size\n";
				this->Err = true; 	
			}
			
			if(Move->Op2->Size != 0 && Move->Op2->Size != Move->Size){
				std::cerr << "basm:" << Move->Op2->Line << ":error: unexpected operand size\n";
				this->Err = true; 	
			}
			break;
		}
		default:
			break;
	}
}

bool Analyzer::ValidCombination(std::shared_ptr<Expr> Op1, std::shared_ptr<Expr> Op2){
	if(Op1->Kind == NodeKind::NUMBER){return false;}	
	return true;
}
