#include "Lexer.h"

Lexer::Lexer (std::string s)
{
	std::cout << "Constructed Lexer" << std::endl;

	inputFile.open (s);

	std::cout << "Opened input file" << std::endl;

	inputFile >> sourceCode;

	sym = sourceCode;
	next ();
/*	std::cout << "Scanned input file" << std::endl;
	while (*sym != '\0') 
	{
		std::cout << *sym << std::endl;
		*sym++;
	}
*/
}

void Lexer::next ()
{
	switch (*sym)
	{
		case '\0':
		{
			std::cout << "Finished!" << std::endl;
			break;
		}
		case 'a'...'z':
		case 'A'...'Z':
		{
			std::cout << "Scanning an identifier:" << std::endl << *sym << std::endl;
			*sym++;
			while (isalnum (*sym))
			{
				std::cout << *sym << std::endl;
				*sym++;
			}
			break;
		}
		case '0'...'9':
		{	
			std::cout << "Starting a number: " << std::endl << *sym << std::endl;
			*sym++;
			while (isdigit (*sym))
			{	
				std::cout << *sym << std::endl;
				*sym++;
			}
			break;
		}
		default:
		{
			std::cout << "Other case: " << *sym << std::endl;
			break;
		}	

	}
}

void Lexer::err ()
{
	std::cout << "Incorrect symbol: " << sym << std::endl;
}
