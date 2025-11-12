#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>


#define BFC_VERSION "0.1"
#define BFC_HELP \
"Usage: [options] file..\n" \
"  --h, --help       Displays this info\n" \
"  --v, --version    Displays the compiler version\n" \
"  -o <file>         Place the output in the file\n" \
"  -ts <size>        Sets the tape size 30000 by default\n" \
"  --sd							 Dumps all non-zero cells at the end of the program\n" \
"  --ha							 Allocates the memory on the heap rather than the stack\n"

int main(int argc, char** argv){
	if(2 > argc){
    std::cerr << "bfc: No input args\n";
    exit(1);
  }

	uint64_t Size = 30000;
	bool StackDump = false;
	bool HeapAlloc = true; 

	std::string FileName; 
  std::string Out = "out.c";

  for(int i = 1; i < argc; ++i){
    if(!strcmp(argv[i], "-o")){
      if(i + 1 >= argc){
        std::cerr << "bfc: Expected an argument after '-o'\n";
        exit(1);
      }else{
        Out = argv[++i];
      }
    }else if(!strcmp(argv[i], "-ts")){
      if(i + 1 >= argc){
        std::cerr << "bfc: Expected an argument after '-ts'\n";
        exit(1);
			}else{
				try{
					Size = std::stoull(argv[++i]);
				}catch(std::invalid_argument& e){
				  std::cerr << "bfc: Invalid size " << argv[i-1] << "\n";
					exit(1);
				}
      }
    }else if(std::filesystem::exists(argv[i])){
			if(std::filesystem::is_directory(argv[i])){
			  std::cerr << "bfc: '" << argv[i] << "' is a directory\n";
        exit(1);
			}
			if(!FileName.empty()){
				std::cerr << "bfc: multiplefiles provided\n";
        exit(1);
			}
			FileName = argv[i];
		}else if(!strcmp(argv[i], "--version") || !strcmp(argv[i], "--v")){
			std::cerr << "bfc version " << BFC_VERSION << "\n";
      exit(0);
		}else if(!strcmp(argv[i], "--help") || !strcmp(argv[i], "--h")){
			std::cerr << BFC_HELP;
      exit(0);
		}else if(!strcmp(argv[i], "--sd")){
			StackDump = true;
		}else if(!strcmp(argv[i], "--ha")){
			HeapAlloc = true;
		}else{
			if(argv[i][0] == '-'){
				std::cerr << "bfc: '" << argv[i] << "' No such flag\n";
        exit(1);
			}else{
				std::cerr << "bfc: '" << argv[i] << "' No such file or directory\n";
        exit(1);
			}
		}
	}

	if(FileName.empty()){
		std::cerr << "bfc: No input files\n";
    exit(1);
  }

	std::string SourceCode; 
	{
		std::ifstream File(FileName);
		if(!File.is_open()){
				std::cerr << "bfc: '" << FileName << "' Error opening file\n";
        exit(1);
		}
		std::string Line; 
		while(std::getline(File, Line)){
			SourceCode+=Line;
		}
	}
	
	std::string Indent = "  ";
	std::vector<std::string> C_Code = {
		"#include <stdio.h>",
		"#include <stdlib.h>",
		"#include <stdint.h>",
		"#include <inttypes.h>",
		"int main(){",
		HeapAlloc ? "  uint8_t* Tape = malloc(" + std::to_string(Size) + ");" : "  uint8_t Tape[" + std::to_string(Size) + "] = {0};" ,
		"  uint64_t Pos = 0;"
	}; 

	uint64_t Pos = 0; 
	int64_t MovPos = 0;
	int64_t ArthCount = 0;

	uint64_t Loop = 0;
	while(Pos < SourceCode.size()){
		switch(SourceCode[Pos]){
			case '+':{
				if(MovPos != 0){
					if(MovPos > 0) C_Code.push_back(Indent + "Pos+=" + std::to_string(MovPos) + ";");
					else C_Code.push_back(Indent + "Pos-=" + std::to_string(std::abs(MovPos)) + ";");
					MovPos = 0;
				}
				ArthCount++;
				Pos++;
				break;
			}
			case '-':{
				if(MovPos != 0){
					if(MovPos > 0) C_Code.push_back(Indent + "Pos+=" + std::to_string(MovPos) + ";");
					else C_Code.push_back(Indent + "Pos-=" + std::to_string(std::abs(MovPos)) + ";");
					MovPos = 0;
				}
				ArthCount--;
				Pos++;
				break;
			}
			case '>':{
				if(ArthCount != 0){
					if(ArthCount > 0) C_Code.push_back(Indent + "Tape[Pos]+=" + std::to_string(ArthCount) + ";");
					else C_Code.push_back(Indent + "Tape[Pos]-=" + std::to_string(std::abs(ArthCount)) + ";");
					ArthCount = 0;
				}
				MovPos++;
				Pos++;
				break;
			}
			case '<':{
				if(ArthCount != 0){
					if(ArthCount > 0) C_Code.push_back(Indent + "Tape[Pos]+=" + std::to_string(ArthCount) + ";");
					else C_Code.push_back(Indent + "Tape[Pos]-=" + std::to_string(std::abs(ArthCount)) + ";");
					ArthCount = 0;
				}
				MovPos--;
				Pos++;
				break;
			}
			case '.':{
				if(ArthCount != 0){
					if(ArthCount > 0) C_Code.push_back(Indent + "Tape[Pos]+=" + std::to_string(ArthCount) + ";");
					else C_Code.push_back(Indent + "Tape[Pos]-=" + std::to_string(std::abs(ArthCount)) + ";");
					ArthCount = 0;
				}
				if(MovPos != 0){
					if(MovPos > 0) C_Code.push_back(Indent + "Pos+=" + std::to_string(MovPos) + ";");
					else C_Code.push_back(Indent + "Pos-=" + std::to_string(std::abs(MovPos)) + ";");
					MovPos = 0;
				}
				C_Code.push_back(Indent + "putchar(Tape[Pos]);");
				Pos++;
				break;
			}
			case ',':{
				if(ArthCount != 0){
					if(ArthCount > 0) C_Code.push_back(Indent + "Tape[Pos]+=" + std::to_string(ArthCount) + ";");
					else C_Code.push_back(Indent + "Tape[Pos]-=" + std::to_string(std::abs(ArthCount)) + ";");
					ArthCount = 0;
				}
				if(MovPos != 0){
					if(MovPos > 0) C_Code.push_back(Indent + "Pos+=" + std::to_string(MovPos) + ";");
					else C_Code.push_back(Indent + "Pos-=" + std::to_string(std::abs(MovPos)) + ";");
					MovPos = 0;
				}
				C_Code.push_back(Indent + "Tape[Pos] = getchar();");
				Pos++;
				break;
			}
			case '[':{
				if(ArthCount != 0){
					if(ArthCount > 0) C_Code.push_back(Indent + "Tape[Pos]+=" + std::to_string(ArthCount) + ";");
					else C_Code.push_back(Indent + "Tape[Pos]-=" + std::to_string(std::abs(ArthCount)) + ";");
					ArthCount = 0;
				}
				if(MovPos != 0){
					if(MovPos > 0) C_Code.push_back(Indent + "Pos+=" + std::to_string(MovPos) + ";");
					else C_Code.push_back(Indent + "Pos-=" + std::to_string(std::abs(MovPos)) + ";");
					MovPos = 0;
				}
				if(SourceCode.size() > Pos + 2 && SourceCode[Pos+1] == '-' && SourceCode[Pos+2] == ']'){
					C_Code.push_back(Indent + "Tape[Pos] = 0;");
					Pos+=3;
				}else{
					C_Code.push_back(Indent + "while(Tape[Pos] != 0){");
					Pos++;
					Indent+="  ";
					Loop++;
				}
				break;
			}
			case ']':{
				if(ArthCount != 0){
					if(ArthCount > 0) C_Code.push_back(Indent + "Tape[Pos]+=" + std::to_string(ArthCount) + ";");
					else C_Code.push_back(Indent + "Tape[Pos]-=" + std::to_string(std::abs(ArthCount)) + ";");
					ArthCount = 0;
				}
				if(MovPos != 0){
					if(MovPos > 0) C_Code.push_back(Indent + "Pos+=" + std::to_string(MovPos) + ";");
					else C_Code.push_back(Indent + "Pos-=" + std::to_string(std::abs(MovPos)) + ";");
					MovPos = 0;
				}
				Indent.erase(Indent.end() - 2, Indent.end());
				C_Code.push_back(Indent + "}");
				Pos++;
				Loop--;
				break;
			}
			default:{
				Pos++;
				break;
			}
		}
	}

	if(Loop != 0){
		std::cerr << "bfc: Invalid bf code [ not closed\n";
    exit(1);
	}

	if(ArthCount != 0){
		if(ArthCount > 0) C_Code.push_back(Indent + "Tape[Pos]+=" + std::to_string(ArthCount) + ";");
		else C_Code.push_back(Indent + "Tape[Pos]-=" + std::to_string(std::abs(ArthCount)) + ";");
		ArthCount = 0;
	}
	if(MovPos != 0){
		if(MovPos > 0) C_Code.push_back(Indent + "Pos+=" + std::to_string(MovPos) + ";");
		else C_Code.push_back(Indent + "Pos-=" + std::to_string(std::abs(MovPos)) + ";");
		MovPos = 0;
	}

	if(StackDump){
		C_Code.push_back("  int p = 0;");
    C_Code.push_back("  for(uint64_t i = 0; i < " + std::to_string(Size) + "; i++){");
    C_Code.push_back("    if(Tape[i] != 0){");
    C_Code.push_back("      printf(\"%\" PRIu64 \" = [%u] \", i, Tape[i]);");
    C_Code.push_back("      p++;");
    C_Code.push_back("    }if(p == 5){printf(\"\\n\"); p = 0;}");
    C_Code.push_back("  }printf(\"\\n\");");
	}
	C_Code.push_back("}");

	std::ofstream CFile(Out);
	for(auto& i : C_Code){
		CFile << i << "\n";
	}
	CFile.close();
}
