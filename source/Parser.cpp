#include "Parser.h"

Parser::Parser (std::string s) : lex (s)
{
	lex = Lexer (s);

}

void Parser::parse ()
{

	computation ();

}

void Parser::computation ()
{
	lex.next ();
	token tk = lex.getToken ();
	if (tk != tk_main) err ();	

	// @@@@ varDecl
	// @@@@ funcDecl

	lex.next ();
	tk = lex.getToken ();
	if (tk != openCurlBracket) err ();
	
	statSequence ();

	lex.next ();
	tk = lex.getToken ();
	if (tk != closeCurlBracket) err ();

	lex.next ();
	tk = lex.getToken ();
	if (tk != dot) err ();
	
}

void Parser::statSequence ()
{
	statement ();

	char c = lex.getSym ();
	while (c == ';')	// exit from the loop when there is no semicolon after the statement
	{
		lex.next ();	// skipping a semicolon
		
		statement ();
	
		c = lex.getSym ();
	}
}

void Parser::statement ()
{
	lex.next ();
	token tk = lex.getToken ();
	switch (tk)
	{
		case tk_let:
		{
			assignment ();

			break;
		}
		case tk_call:
		{
			funcCall ();

			break;
		}
		case tk_if:
		{
			ifStatement ();

			break;
		}
		case tk_while:
		{
			whileStatement ();

			break;
		}
		case tk_return:
		{
			returnStatement ();

			break;
		}
		default:
		{
			std::cout << "Incorrect token in a statement function: " << tk << std::endl;
			err ();
			break;
		}
	}
}

void Parser::assignment ()
{
	/**
	*	First token (tk_let) is already scanned
	*/

	designator ();

	lex.next ();
	token tk = lex.getToken ();
	if (tk != assign) err ();
	
	expression ();
}

void Parser::funcCall ()
{
	/**
	*	First token (tk_call) is already scanned
	*/
	
	ident ();

	char c = lex.getSym ();
	token tk;
	if (c == '(')
	{
		lex.next ();	// skipping opening bracket
		c = lex.getSym ();
		if (c != ')')
		{
			expression ();
			
			c = lex.getSym ();
			while (c == ',') 
			{
				lex.next ();	// skipping a comma

				expression ();

				c = lex.getSym ();
			}
		}
		
		lex.next ();	// skipping closing bracket
		tk = lex.getToken ();
		if (tk != closeBracket) err();
	}
}

void Parser::ifStatement ()
{
	/**
	*	First token (tk_if) is already scanned
	*/

	relation ();

	lex.next ();
	token tk = lex.getToken ();
	if (tk != tk_then) err ();
	
	statSequence ();

	lex.next ();
	tk = lex.getToken ();
	if (tk == tk_else)
	{
		statSequence ();
	}
	
	lex.next ();
	tk = lex.getToken ();
	if (tk != tk_fi) err();
}

void Parser::whileStatement ()
{
	/**
	*	First token (tk_while) is already scanned
	*/

	relation ();

	lex.next ();
	token tk = lex.getToken ();
	if (tk != tk_do) err ();

	statSequence ();

	lex.next ();
	tk = lex.getToken ();
	if (tk != tk_od) err ();
}

void Parser::returnStatement ()
{
	/**
	*	First token (tk_return) is already scanned
	*/

	expression ();
}

void Parser::designator ()
{
	ident ();
	
	char c = lex.getSym ();
	token tk;
	while (c == '[')
	{
		lex.next ();	// skipping openSqBracket
		
		expression ();

		lex.next ();	// skipping closeSqBracket
		tk = lex.getToken ();
		if (tk != closeSqBracket) err();
		c = lex.getSym ();
	}
}

void Parser::expression ()
{
	term ();

	lex.next ();
	token tk = lex.getToken ();
	if (tk == add)
	{
	}
	else if (tk == sub)
	{
	}
	else err();

	term ();
}

void Parser::ident ()
{
	lex.next ();
	token tk = lex.getToken ();
	if (tk != identifier) err ();
}

void Parser::relation ()
{
	expression ();

	lex.next ();
	token tk = lex.getToken ();
	switch (tk)
	{
		case eq:
		{
			break;
		}
		case neq:
		{
			break;
		}
		case less:
		{
			break;
		}
		case le:
		{
			break;
		}
		case greater:
		{
			break;
		}
		case ge:
		{
			break;
		}
		default:
		{
			std::cout << "Incorrect token in a relation function: " << tk << std::endl;
			err ();
			break;
		}	
	}

	expression ();
}

void Parser::term ()
{
	factor ();

	lex.next ();
	token tk = lex.getToken ();
	if (tk == mul)
	{
	}
	else if (tk == divis)
	{
	}
	else err ();

	factor ();
}

void Parser::factor ()
{
	char c = lex.getSym ();
	token tk;
	if (isalpha (c))
	{
		designator ();
	}
	else if (isdigit (c))
	{
		number ();
	}
	else if (c == '(')
	{
		lex.next ();	//  skipping opening bracket
		
		expression ();

		lex.next ();
		tk = lex.getToken ();
		if (tk != closeBracket) err ();
	}
	else
	{
		err ();
	}
	//@@@@	funcCall!!!
}

void Parser::number ()
{
	lex.next ();
	token tk = lex.getToken ();
	if (tk != num) err ();
}

void Parser::err ()
{
	std::cout << "Error in a Parser" << std::endl;
}
