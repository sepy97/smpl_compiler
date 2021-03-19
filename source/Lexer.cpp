#include "Lexer.h"

Lexer::Lexer (std::string s)
{
	inputFile.open (s);
	val = 0;
	id = 0;
	numOfVars = 1;

	tk = eof;

	sym = inputFile.get ();
}

void Lexer::next ()
{
    while (sym == ' ' || sym == '\n' || sym == '\t' || sym == '\r')
    {
        sym = inputFile.get ();
    }
    
	switch (sym)
	{
		case EOF:
		{
            tk = eof;
			break;
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
			
			if (reservedTokens.find (var) != reservedTokens.end ())
			{
				tk = reservedTokens[var];
			}
			else
			{
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

				tk = identifier;
			}
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
			tk = num;
			break;
		}
		case '=':
		{
			sym = inputFile.get ();
			if (sym == '=')
			{
				sym = inputFile.get ();
				tk = eq;
			}
			else err ();
			break;
		}
		case '!':
		{
			sym = inputFile.get ();
			if (sym == '=')
			{
				sym = inputFile.get ();
				tk = neq;
			}
			else err ();
			break;
		}
		case '<':
		{
			sym = inputFile.get ();
			if (sym == '=')
			{
				sym = inputFile.get ();
				tk = le;
			}
			else if (sym == '-')
			{
				sym = inputFile.get ();
				tk = assign;
			}
			else tk = less;
			break;
		}
		case '>':
		{
			sym = inputFile.get ();
			if (sym == '=')
			{
				sym = inputFile.get ();
				tk = ge;
			}
			else tk = greater;
			break;
		}
		case '+':
		{
			sym = inputFile.get ();
			tk = add;
			break;
		}
		case '-':
		{
			sym = inputFile.get ();
			tk = sub;
			break;
		}
		case '*':
		{
			sym = inputFile.get ();
			tk = mul;
			break;
		}
		case '/':
		{
			sym = inputFile.get ();
			tk = divis;
			break;
		}
		case '(':
		{
			sym = inputFile.get ();
			tk = openBracket;
			break;
		}
		case ')':
		{
			sym = inputFile.get ();
			tk = closeBracket;
			break;
		}
		case '[':
		{
			sym = inputFile.get ();
			tk = openSqBracket;
			break;
		}
		case ']':
		{
			sym = inputFile.get ();
			tk = closeSqBracket;
			break;
		}
		case '{':
		{
			sym = inputFile.get ();
			tk = openCurlBracket;
			break;
		}
		case '}':
		{
			sym = inputFile.get ();
			tk = closeCurlBracket;
			break;
		}
		case '.':
		{
			sym = inputFile.get ();
			tk = dot;
			break;	
		}
		case ',':
		{
			sym = inputFile.get ();
			tk = comma;
			break;
		}
		case ';':
		{
			sym = inputFile.get ();
			tk = semicolon;
			break;
		}
		case '#':
		{
			sym = inputFile.get ();
			while (sym != '\n') sym = inputFile.get ();
			sym = inputFile.get ();
			break;
		}		
		default:
		{
            err ();
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

char Lexer::getSym ()
{
	return sym;
}

void Lexer::err ()
{
	std::cout << "Incorrect symbol: " << sym << std::endl;
}
