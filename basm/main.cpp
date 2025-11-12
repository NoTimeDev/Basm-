#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "Analyzer/Analyzer.hpp"
#include "Lexer/Lexer.hpp"
#include "Parser/Ast.hpp"
#include "Parser/Parser.hpp"
#include "CodeGen/CodeGen.hpp"

#define BASM_VERSION "0.1"
#define BASM_HELP \
"Usage: [options] file..\n" \
"  --h, --help       Displays this info\n" \
"  --v, --version    Displays the compiler version\n" \
"  -o <file>         Place the output in the file\n"

int main(int argc, char** argv){
  if(2 > argc){
    std::cerr << "basm: No input args\n";
    exit(1);
  }


  std::string FileName; 
  std::string Out = "out.bf";

  for(int i = 1; i < argc; ++i){
    if(!strcmp(argv[i], "-o")){
      if(i + 1 >= argc){
        std::cerr << "basm: Expected an argument after '-o'\n";
        exit(1);
      }else{
        Out = argv[++i];
      }
    }else if(std::filesystem::exists(argv[i])){
			if(std::filesystem::is_directory(argv[i])){
			  std::cerr << "basm: '" << argv[i] << "' is a directory\n";
        exit(1);
			}
			if(!FileName.empty()){
				std::cerr << "basm: multiplefiles provided\n";
        exit(1);
			}
			FileName = argv[i];
		}else if(!strcmp(argv[i], "--version") || !strcmp(argv[i], "--v")){
			std::cerr << "basm version " << BASM_VERSION << "\n";
      exit(0);
		}else if(!strcmp(argv[i], "--help") || !strcmp(argv[i], "--h")){
			std::cerr << BASM_HELP;
      exit(0);
		}else{
			if(argv[i][0] == '-'){
				std::cerr << "basm: '" << argv[i] << "' No such flag\n";
        exit(1);
			}else{
				std::cerr << "basm: '" << argv[i] << "' No such file or directory\n";
        exit(1);
			}
		}
	}
	
	if(FileName.empty()){
		std::cerr << "basm: No input files\n";
    exit(1);
  }

	std::string Contents; 
	{
		std::ifstream File(FileName);
		if(!File.is_open()){
				std::cerr << "basm: '" << FileName << "' Error opening file\n";
        exit(1);
		}
		std::string Line; 
		while(std::getline(File, Line)){
			Contents+=Line+"\n";
		}
	}

	std::vector<Token> Tokens;
	Lex(Tokens, Contents);
	for(auto& i : Tokens){
		std::cout << i << "\n";
	}
	
	Parser Psr(Tokens);
	ProgramNode Ast = Psr.Parse();
	for(auto& i : Ast.Body){i->Print(0); std::cout << "\n";}

	Analyzer Ans(Ast);
	Ans.Analyze();

	CodeGen Cg(Ast, Ans.Symbols);
	Cg.Gen(Out);	
}

