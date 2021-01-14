#include <string>
#include <iostream>
#include <fstream>
#include <cctype>
#include <vector>

enum token
{
	identifier,
	number,
	
	eof
};

class Lexer
{
public:
	Lexer (std::string s);
	std::vector<token> tokenize ();

private:
	char* sym;
	void err ();
	void next ();
	std::ifstream inputFile;
	char* sourceCode;

};
