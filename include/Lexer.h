#include <string>
#include <iostream>
#include <fstream>
#include <cctype>
#include <vector>
#include <map>
#include <unordered_map>

enum token
{
	identifier,
	number,
	eq,
	neq,
	le,
	ge,
	less,
	greater,
	add,
	sub,
	mul,
	div,
	append,

	openBracket,
	closeBracket,
	openSqBracket,
	closeSqBracket,
	openCurlBracket,
	closeCurlBracket,

	dot,
	comma,
	semicolon,

	tk_let,
	tk_call,
	tk_if,
	tk_then,
	tk_else,
	tk_fi,
	tk_while,
	tk_do,
	tk_od,
	tk_return,
	tk_var,
	tk_array,
	tk_void,
	tk_function,
	tk_main,
	
	eof
};

class Lexer
{
public:
	Lexer (std::string s);
	void next ();
	int getVal ();
	int getId ();
	token getToken ();

private:
	char sym;
	void err ();
	std::ifstream inputFile;
	
	int val;
	int id;
	token tk;
	
	typedef std::map<std::string, int> map;

	unsigned numOfVars;
	map varTable;

	std::unordered_map<std::string, token> reservedTokens = {
	{"let", tk_let},
	{"call", tk_call},
	{"if", tk_if},
	{"then", tk_then},
	{"else", tk_else},
	{"fi", tk_fi},
	{"while", tk_while},
	{"do", tk_do},
	{"od", tk_od},
	{"return", tk_return},
	{"var", tk_var},
	{"array", tk_array},
	{"void", tk_void},
	{"function", tk_function},
	{"main", tk_main} };

};
