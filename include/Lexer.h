#include <string>
#include <iostream>
#include <fstream>
#include <cctype>
#include <vector>
#include <map>
#include <unordered_map>

enum token
{
	identifier, //0
	num,    //1
	eq,     //2
	neq,    //3
	le,     //4
	ge,     //5
	less,   //6
	greater,//7
	add,    //8
	sub,    //9
	mul,    //10
	divis,  //11
	assign, //12

	openBracket,    //13
	closeBracket,   //14
	openSqBracket,   //15
	closeSqBracket,   //16
	openCurlBracket,   //17
	closeCurlBracket,   //18

	dot,   //19
	comma,   //20
	semicolon,   //21

	tk_let,   //22
	tk_call,   //23
	tk_if,   //24
	tk_then,   //25
	tk_else,   //26
	tk_fi,   //27
	tk_while,   //28
	tk_do,   //29
	tk_od,   //30
	tk_return,   //31
	tk_var,   //32
	tk_array,   //33
	tk_void,   //34
	tk_function,   //35
	tk_main,   //36
    tk_read,   //37
    tk_write,   //38
    tk_writeNL,   //39
	
	eof   //40
};

class Lexer
{
public:
	Lexer (std::string s);
	void next ();
	int getVal ();
	int getId ();
	token getToken ();
	char getSym ();

private:
   // const char EOF = -1;
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
	{"main", tk_main},
    {"InputNum", tk_read},
    {"OutputNum", tk_write},
    {"OutputNewLine", tk_writeNL}
    };

};
