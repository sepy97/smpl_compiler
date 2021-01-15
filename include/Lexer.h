#include <string>
#include <iostream>
#include <fstream>
#include <cctype>
#include <vector>
#include <map>


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

};
