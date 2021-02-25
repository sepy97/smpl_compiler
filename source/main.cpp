#include <string>
#include "Parser.h"

std::string fileToCompile;

int main (int argc, char* argv[])
{
    
    for (int i = 1; i < argc; i++)
    {
        fileToCompile = argv[i];
        
        Parser tmp (fileToCompile);
        tmp.parse ();
        //std::cout << "STARTED DOT GRAPH" << std::endl;
        tmp.dotGraph ();
        //std::cout << "FINISHED DOT GRAPH" << std::endl;
    }
	//Lexer tmp ("inp.txt");
    //Parser tmp ("inp.txt");
    
    //std::cout << "BEFORE parse" << std::endl;
    
    //tmp.parse ();
/*
	std::cout << "Lexer constructed" << std::endl;

	tmp.next ();
	std::cout << "next function called: " << tmp.getToken () << " " << tmp.getId () << " " << tmp.getVal () << std::endl;
	tmp.next ();
	std::cout << "next function called: " << tmp.getToken () << " " << tmp.getId () << " " << tmp.getVal () << std::endl;
	tmp.next ();
	std::cout << "next function called: " << tmp.getToken () << " " << tmp.getId () << " " << tmp.getVal () << std::endl;
*/

//	return 0;
}
