#include <string>
#include "Parser.h"

std::string filetoCompile;

int main ()
{
	//Lexer tmp ("inp.txt");
    Parser tmp ("inp.txt");
    
    //std::cout << "BEFORE parse" << std::endl;
    
    tmp.parse ();
/*
	std::cout << "Lexer constructed" << std::endl;

	tmp.next ();
	std::cout << "next function called: " << tmp.getToken () << " " << tmp.getId () << " " << tmp.getVal () << std::endl;
	tmp.next ();
	std::cout << "next function called: " << tmp.getToken () << " " << tmp.getId () << " " << tmp.getVal () << std::endl;
	tmp.next ();
	std::cout << "next function called: " << tmp.getToken () << " " << tmp.getId () << " " << tmp.getVal () << std::endl;
*/

	return 0;
}
