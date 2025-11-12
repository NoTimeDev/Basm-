#pragma once 

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>
#include "../Parser/Ast.hpp"

class Analyzer{
	std::vector<std::shared_ptr<Stmt>>& Body;
	bool Err = false;
public:
	uint64_t Id = 0;
	std::unordered_map<std::string, uint64_t> Symbols;
	
	Analyzer(ProgramNode& Body);
	void Analyze();
	void AnalyzeStmt(std::shared_ptr<Stmt> Node);
	bool ValidCombination(std::shared_ptr<Expr> Op1, std::shared_ptr<Expr> Op2);
};
