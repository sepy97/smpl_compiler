#include <string>
#include "Lexer.h"

std::string filetoCompile;

int main ()
{
	Lexer tmp ("inp.txt");

	std::cout << "Lexer constructed" << std::endl;

	tmp.next ();
	tmp.next ();
	
	std::cout << "next function called: " << tmp.getToken () << " " << tmp.getId () << std::endl;


	return 0;
}
