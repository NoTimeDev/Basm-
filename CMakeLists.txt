cmake_minimum_required(VERSION 4.1)

project(basm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wextra -Werror -g)

set(BASM_HEADER_FILES
	./basm/Lexer/Lexer.hpp
	./basm/Parser/Ast.hpp
	./basm/Parser/Parser.hpp
	./basm/Analyzer/Analyzer.hpp
	./basm/CodeGen/CodeGen.hpp
)

set(BASM_SOURCE_FILES
	./basm/main.cpp
	./basm/Lexer/Lexer.cpp
	./basm/Parser/Parser.cpp
	./basm/Parser/Ast.cpp
	./basm/Analyzer/Analyzer.cpp
	./basm/CodeGen/CodeGen.cpp
)
set(BFC_SOURCE_FILES
	./bfc/main.cpp
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(basm ${BASM_HEADER_FILES} ${BASM_SOURCE_FILES})
add_executable(bfc ${BFC_HEADER_FILES} ${BFC_SOURCE_FILES})

#target_link_libraries() 
