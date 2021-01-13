.DEFAULT_GOAL := compiler

compiler: ./source/Lexer.cpp ./source/main.cpp
	clang++ -o compiler ./source/main.cpp ./source/Lexer.cpp -I ./include
