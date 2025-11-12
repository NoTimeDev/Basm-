#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "../Parser/Ast.hpp"

class BfCode{
public:
	uint64_t Pos = 0;
	std::vector<char> Code;
	void Add(uint64_t count, bool Term = 0);
	void Sub(uint64_t count, bool Term = 0);
	void MoveR(uint64_t count, bool Term = 0);
	void MoveL(uint64_t count, bool Term = 0);
	void OpenLoop(bool Term = 0);
	void CloseLoop(bool Term = 0);
	void MoveRel(uint64_t pos, bool Term = 0);
	void Print(bool Term = 0);
	BfCode();
};

struct Temporary{
	uint16_t Size;
	uint16_t Pos;
};

class CodeGen{
	std::vector<std::shared_ptr<Stmt>>& Body;
	ProgramNode& Prog;

	std::unordered_map<std::string, uint64_t>& SymTable;
	std::vector<BfCode> Blocks;
	std::vector<Temporary> Temps;

	BfCode Code;

	std::vector<char> Bytes;

	uint16_t Imov;
	uint16_t Reg;
	uint16_t Spec;
	uint16_t Temp;
public:
	CodeGen(ProgramNode& Body, std::unordered_map<std::string, uint64_t>& SymTable);
	void GenStmt(std::shared_ptr<Stmt> Node);
	void Gen(std::string Name);

	void Add(uint64_t Pos1, uint64_t Pos2);
	void Copy(uint64_t dest, uint64_t src);
	void Mth(uint64_t Pos1, uint64_t Pos2, uint64_t Res);
};
