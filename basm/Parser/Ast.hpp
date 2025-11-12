#pragma  once 

#include <cstdint>
#include <memory>
#include <vector>
#include <string>

enum class NodeKind{
	BLOCK,
	ADD,
	MOV, 

	REGISTER, 
	NUMBER, 
	NULL_, 
};

struct Stmt{
	Stmt();
	NodeKind Kind;
	uint64_t Line;
	virtual void Print(int indent);
};

struct Expr{
	Expr();
	uint8_t Size; 
	uint64_t Line;
	NodeKind Kind;
	virtual void Print(int indent);
};

struct ProgramNode{
	std::vector<std::shared_ptr<Stmt>> Body;
	uint8_t MaxReg = 0;
};


struct NullStmt  : public Stmt{
	void Print(int indent) override;
	NullStmt();
};


struct BlockStmt : public Stmt{
	void Print(int indent) override;
	BlockStmt(uint64_t Line, std::string Name, std::vector<std::shared_ptr<Stmt>> Body);
	std::string Name; 
	std::vector<std::shared_ptr<Stmt>> Body;
};

struct InstructionStmt : public Stmt{
	void Print(int indent) override;
	InstructionStmt(NodeKind Kind, uint64_t Line, std::shared_ptr<Expr> Op1, std::shared_ptr<Expr> Op2, uint8_t Size);
	std::shared_ptr<Expr> Op1; 
	std::shared_ptr<Expr> Op2; 
	uint8_t Size;
};


//Expr 
struct NullExpr : public Expr{
	void Print(int indent) override;
	NullExpr();
};

struct NumberExpr : public Expr{
	void Print(int indent) override;
	NumberExpr(uint64_t Line, uint64_t Value);

	uint64_t Value;
};

struct RegisterExpr : public Expr{
	void Print(int indent) override;
	RegisterExpr(uint64_t Line, uint8_t Id, uint8_t Size);
	
	uint8_t Id; 
};
