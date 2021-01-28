#include "Parser.h"

Parser::Parser (std::string s) : lex (s)
{
	//lex = Lexer (s);
    sp = 0;
}

void Parser::parse ()
{
    
    lex.next ();
    token tk = lex.getToken ();
    if (tk != tk_main) err ("parse");
    else computation ();

}

void Parser::computation ()
{
    token tk = lex.getToken ();
	if (tk != tk_main) err ("computation");
    lex.next ();                    /** Consuming tk_main */
    
    tk = lex.getToken ();
    while (tk == tk_var || tk == tk_array)
    {
        varDecl ();
        
        tk = lex.getToken ();
    }
    
    while (tk == tk_void || tk == tk_function)
    {
        funcDecl ();
        
        tk = lex.getToken ();
    }
	tk = lex.getToken ();
	if (tk != openCurlBracket) err ("computation");
    lex.next ();                    /** Consuming bracket */
    
	statSequence ();

	tk = lex.getToken ();
	if (tk != closeCurlBracket) err ("computation");
    lex.next ();                    /** Consuming bracket */
    
	tk = lex.getToken ();
	if (tk != dot) err ("computation");
    lex.next ();                    /** Consuming dot */
    
    tk = lex.getToken ();
    if (tk != eof) err ("computation");          /** tk should be eof */
}

void Parser::varDecl ()
{
    typeDecl ();
    
    token tk = lex.getToken ();
    while (tk == identifier)
    {
        ident ();
        
        tk = lex.getToken ();
        if (tk == comma)
        {
            lex.next ();            /** Consuming comma */
            tk = lex.getToken ();
        }
        else if (tk == semicolon)
        {
            break;
        }
        else
        {
            err ("varDecl");
            break;
        }
    }
    
    if (tk != semicolon) err ("varDecl");
    lex.next ();                    /** Consuming semicolon */
}

void Parser::typeDecl ()
{
    token tk = lex.getToken ();
    if (tk == tk_var)
    {
        lex.next ();                /** Consuming var */
    }
    else if (tk == tk_array)
    {
        lex.next ();                /** Consuming array */
        tk = lex.getToken ();
        while (tk == openSqBracket)
        {
            lex.next ();            /** Consuming square bracket */
            tk = lex.getToken ();
            if (tk != num) err ("typeDecl");
            
            number ();
            
            tk = lex.getToken ();
            if (tk != closeSqBracket) err ("typeDecl");
            lex.next ();            /** Consuming square bracket */
            tk = lex.getToken ();
        }
    }
}

void Parser::funcDecl ()
{
    token tk = lex.getToken ();
    if (tk == tk_void)
    {
        lex.next ();                /** Consuming tk_void */
        tk = lex.getToken ();
    }
    if (tk != tk_function) err ("funcDecl");
    lex.next ();                    /** Consuming tk_function */
    
    ident ();
    
    formalParam ();
    
    tk = lex.getToken ();
    if (tk != semicolon) err ("funcDecl");
    lex.next ();                    /** Consuming semicolon */
    
    funcBody ();
    
    tk = lex.getToken ();
    if (tk != semicolon) err ("funcDecl");
    lex.next ();                    /** Consuming semicolon */
}

void Parser::formalParam ()
{
    token tk = lex.getToken ();
    if (tk != openBracket) err ("formalParam");
    lex.next ();                    /** Consuming bracket */
    
    tk = lex.getToken ();
    while (tk == identifier)
    {
        ident ();
        
        tk = lex.getToken ();
        if (tk == comma)
        {
            lex.next ();            /** Consuming comma */
            tk = lex.getToken ();
        }
        else if (tk == closeBracket)
        {
            break;
        }
        else
        {
            err ("formalParam");
            break;
        }
    }
    
    tk = lex.getToken ();
    if (tk != closeBracket) err ("formalParam");
    lex.next ();                   /** Consuming bracket */
}

void Parser::funcBody ()
{
    token tk = lex.getToken ();
    while (tk == tk_var || tk == tk_array)
    {
        varDecl ();
        
        tk = lex.getToken ();
    }
    
    tk = lex.getToken ();
    if (tk != openCurlBracket) err ("funcBody");
    lex.next ();                    /** Consuming curl bracket */
    tk = lex.getToken ();
    if (tk == tk_let || tk == tk_call || tk == tk_if || tk == tk_while || tk == tk_return)
    {
        statSequence ();
        
    }
    tk = lex.getToken ();
    if (tk != closeCurlBracket) err ("funcBody");
    lex.next ();                    /** Consuming curl bracket */
}

void Parser::statSequence ()
{
    token tk = lex.getToken ();
    while (tk == tk_let || tk == tk_call || tk == tk_if || tk == tk_while || tk == tk_return)
    {
        statement ();
        
        tk = lex.getToken ();
        if (tk == semicolon)
        {
            lex.next ();            /** Consuming semicolon */
            tk = lex.getToken ();
        }
    }
}

void Parser::statement ()
{
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
			err ("statement");
			break;
		}
	}
}

void Parser::assignment ()
{
    lex.next ();                    /** Consuming let */
    
	designator ();

	token tk = lex.getToken ();
	if (tk != assign) err ("assignment");
    lex.next ();                    /** Consuming assign */
	
	expression ();
    
}

void Parser::funcCall ()
{
    lex.next ();                    /** Consuming call */
	
	ident ();
    
    token tk = lex.getToken ();
    if (tk == openBracket)
    {
        lex.next ();                /** Consuming bracket */
        tk = lex.getToken ();
        while (tk == identifier || tk == num || tk == openBracket || tk == tk_call)
        {
            expression ();
            
            tk = lex.getToken ();
            if (tk == comma)
            {
                lex.next ();        /** Consuming comma */
                tk = lex.getToken ();
            }
        }
        tk = lex.getToken ();
        if (tk != closeBracket) err ("funcCall");
        lex.next ();                /** Consuming bracket */
    }
}

void Parser::ifStatement ()
{
    lex.next ();                    /** Consuming if */

	relation ();

	token tk = lex.getToken ();
	if (tk != tk_then) err ("ifStatement");
    lex.next ();                    /** Consuming then */
	
	statSequence ();

	tk = lex.getToken ();
	if (tk == tk_else)
	{
        lex.next ();                /** Consuming else */
        
		statSequence ();
	}
	
	tk = lex.getToken ();
	if (tk != tk_fi) err ("ifStatement");
    lex.next ();                    /** Consuming fi */
}

void Parser::whileStatement ()
{
    lex.next ();                    /** Consuming while */

	relation ();

	token tk = lex.getToken ();
	if (tk != tk_do) err ("whileStatement");
    lex.next ();                    /** Consuming do */

	statSequence ();

	tk = lex.getToken ();
	if (tk != tk_od) err ("whileStatement");
    lex.next ();                    /** Consuming od */
}

void Parser::returnStatement ()
{
    lex.next ();                    /** Consuming return */
    token tk = lex.getToken ();
    if (tk == identifier || tk == num || tk == openBracket || tk == tk_call)
    {
        expression ();
        
    }
}

void Parser::designator ()
{
	ident ();
    
    token tk = lex.getToken ();
    while (tk == openSqBracket)
    {
        lex.next ();                /** Consuming square bracket */
        
        expression ();
        
        tk = lex.getToken ();
        if (tk != closeSqBracket) err ("designator");
        lex.next ();                /** Consuming square bracket */
        tk = lex.getToken ();
    }
}

void Parser::expression ()
{
	term ();

	token tk = lex.getToken ();
    //if (tk != add && tk != sub) err ("expression");
    while (tk == add || tk == sub)
    {
        if (tk == add)
        {
            lex.next ();                /** Consuming add */
        }
        else if (tk == sub)
        {
            lex.next ();                /** Consuming sub */
        }
        
        term ();
        
        tk = lex.getToken ();
    }
}

void Parser::ident ()
{
	token tk = lex.getToken ();
	if (tk != identifier) err ("ident");
    lex.next ();                        /** Consuming id */
}

void Parser::relation ()
{
	expression ();
    
    relOp ();

	expression ();
    
}

void Parser::relOp ()
{
    token tk = lex.getToken ();
    switch (tk)
    {
        case eq:
        {
            lex.next ();                /** Consuming eq */
            break;
        }
        case neq:
        {
            lex.next ();                /** Consuming neq */
            break;
        }
        case less:
        {
            lex.next ();                /** Consuming less */
            break;
        }
        case le:
        {
            lex.next ();                /** Consuming le */
            break;
        }
        case greater:
        {
            lex.next ();                /** Consuming greater */
            break;
        }
        case ge:
        {
            lex.next ();                /** Consuming ge */
            break;
        }
        default:
        {
            err ("relOp");
            break;
        }
    }
}

void Parser::term ()
{
	factor ();

	token tk = lex.getToken ();
    
    while (tk == mul || tk == divis)
    {
        if (tk == mul)
        {
            lex.next ();                /** Consuming mul */
        }
        else if (tk == divis)
        {
            lex.next ();                /** Consuming divis */
        }
        
        factor ();
        
        tk = lex.getToken ();
    }
}

void Parser::factor ()
{
    token tk = lex.getToken ();
    switch (tk)
    {
        case identifier:
        {
            designator ();
            
            break;
        }
        case num:
        {
            number ();
            
            break;
        }
        case openBracket:
        {
            lex.next ();                /** Consuming bracket */
            
            expression ();
            
            tk = lex.getToken ();
            if (tk != closeBracket) err ("factor");
            lex.next ();                /** Consuming bracket */
            break;
        }
        case tk_call:
        {
            funcCall ();
            
            break;
        }
        default:
        {
            err ("factor");
            break;
        }
    }
}

void Parser::number ()
{
	token tk = lex.getToken ();
	if (tk != num) err ("number");
    lex.next ();
}

void Parser::err (std::string arg)
{
	std::cout << "Error in a Parser" << std::endl;
    std::cout << "Function: " << arg << std::endl;
}
