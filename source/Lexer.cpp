#include "Lexer.h"

Lexer::Lexer (std::string s)
{
	std::cout << "Constructor called" << std::endl;

	inputFile.open (s);

	std::cout << "inpfile opened" << std::endl;

	val = 0;
	id = 0;
	numOfVars = 1;

	tk = eof;

	sym = inputFile.get ();
}

void Lexer::next ()
{
	switch (sym)
	{
		case '\0':
		{
			std::cout << "Finished!" << std::endl;
			break;
		}
		case ' ':
		{
			sym = inputFile.get ();
			while (sym == ' ') sym = inputFile.get ();
		}
		case 'a'...'z':
		case 'A'...'Z':
		{
			std::string var;
			var += sym;
			sym = inputFile.get ();
			while (isalnum (sym))
			{
				var += sym;
				sym = inputFile.get ();
			}
			
			if (varTable.find (var) == varTable.end ())
			{
				varTable.insert (std::pair<std::string, int> (var, numOfVars));
				id = numOfVars;
				numOfVars++;
			}
			else
			{
				id = varTable[var];
			}

			//id = -1; @@@
			
			tk = identifier;
			break;
		}
		case '0'...'9':
		{	
			int v = sym - '0';
			sym = inputFile.get ();
			while (isdigit (sym))
			{	
				v = 10 * v + sym - '0';
				sym = inputFile.get ();
			}
			val = v;
			tk = number;
			break;
		}
		default:
		{
			std::cout << "Other case: " << sym << std::endl;
			break;
		}	

	}

}

int Lexer::getVal ()
{
	return val;
}

int Lexer::getId ()
{
	return id;
}

token Lexer::getToken ()
{
	return tk;
}

void Lexer::err ()
{
	std::cout << "Incorrect symbol: " << sym << std::endl;
}
