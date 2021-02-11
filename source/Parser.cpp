#include "Parser.h"

Parser::Parser (std::string s) : lex (s)
{
	//lex = Lexer (s);
    sp = 0;
    
    //std::cout << "BEFORE Module ctor" << std::endl;
    
    this->m = new Module ();
    
    //std::cout << "AFTER Module ctor" << std::endl;
}

void Parser::parse ()
{
    
    lex.next ();
    token tk = lex.getToken ();
    if (tk != tk_main) err ("parse");
    else
    {
       // Instruction instr = new Instruction (++sp);
        BasicBlock* bb = new BasicBlock ();///*instr*/new Instruction (++sp));
        Function* f = new Function (std::string ("main"), bb);
        m->insertFunc (f);
        
        currentFunc = f;
        currentBB = bb;
        
        //std::cout << "BEFORE computation" << std::endl;
        
        computation ();
        
        //std::cout << "AFTER computation" << std::endl;
        //std::cout << m->body.size () << std::endl;
        
        std::cout << m->toString () << std::endl;
        /*
         for (auto& t : varTable)
            std::cout << t.first << " " << t.second << std::endl;
        */
    }

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
        Operand o = ident ();
        if (o.getKind () == variable)
        {
            varTable [o.getVal ()] = 0; //Marking uninitialized variables with ssa line 0
        }
        else err ("Incorrect variable declaration");
                             
        
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
    
    if (tk != semicolon) err ("Syntax error in variable declaration: missing semicolon");
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
    
	Operand o1 = designator ();
    if (o1.getKind () != variable) err ("Incorrect assignment");

	token tk = lex.getToken ();
	if (tk != assign) err ("assignment");
    lex.next ();                    /** Consuming assign */
	
	Operand o2 = expression ();
    if (o2.getKind () == constant)
    {
        int line = ++sp;
        currentBB->pushInstruction (new Instruction (op_add, Operand (0), o2, line) );
        varTable [o1.getVal ()] = line;
    }
    else if (o2.getKind () == variable)
    {
        // assign variable's ssa line (from varTable) to o1
        int line = varTable [o2.getVal ()];
        if (line > 0)
        {
            varTable [o1.getVal ()] = line;
        }
        else err ("Assigning an unitialized variable");
    }
    else if (o2.getKind () == type_read)
    {
        
    }
    else if (o2.getKind () == type_write)
    {
        
    }
    else // o2 is SSA line
    {
        if (o2.getVal () > 0)
        {
            varTable [o1.getVal ()] = o2.getVal ();
        }
        else err ("Assigning non-existing SSA line");
    }
    
    
}

Operand Parser::funcCall ()
{
    lex.next ();                    /** Consuming call */
    //tk = lex.getToken ();
	Operand o1 = ident ();
    if (o1.getKind () == type_read)
    {
        lex.next ();                /** Consuming bracket */
        lex.next ();                /** Consuming bracket */
        
        //std::cout << sp << std::endl;
        
        int line = ++sp;
        
        //std::cout << line << std::endl;
        //std::cout << sp << std::endl;
        
        currentBB->pushInstruction (new Instruction (op_read, o1, Operand (), line) );
        o1 = Operand (SSALine, line);
    }
    else if (o1.getKind () == type_write)
    {
        lex.next ();                /** Consuming bracket */
        //Operand o2 =
        Operand o2 = ident ();
        
        //std::cout << sp << std::endl;
        
        int line = ++sp;
        
        //std::cout << line << std::endl;
        //std::cout << sp << std::endl;
        
        if (o2.getKind () == variable)
        {
            o2 = Operand (SSALine, varTable [o2.getVal ()]);
        }
        currentBB->pushInstruction (new Instruction (op_write, o2, Operand (), line) );
        o1 = Operand (SSALine, line);
        
        lex.next ();                /** Consuming bracket */
        
    }
    else
    {
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
    
    return o1;
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

Operand Parser::designator ()
{
	Operand o1 = ident ();
    
    token tk = lex.getToken ();
    while (tk == openSqBracket)
    {
        lex.next ();                /** Consuming square bracket */
        
        expression ();              //@@@@  only static arrays?..
        
        tk = lex.getToken ();
        if (tk != closeSqBracket) err ("designator");
        lex.next ();                /** Consuming square bracket */
        tk = lex.getToken ();
    }
    
    return o1;
}

Operand Parser::expression ()
{
	Operand o1 = term ();
    //resultOp rop;

	token tk = lex.getToken ();
    //if (tk != add && tk != sub) err ("expression");
    while (tk == add || tk == sub)
    {
        if (tk == add)
        {
            //rop = resultAdd;
            lex.next ();                /** Consuming add */
            Operand o2 = term ();
            
            if (o1.getKind () == constant && o2.getKind () == constant)
            {
                o1 = Operand (constant, o1.getVal () + o2.getVal ());
            }
            else
            {
                opCode opc = op_add;
                //int operand1, operand2;
                int line = ++sp;
                if (o1.getKind () == variable)
                {
                    o1 = Operand (SSALine, varTable [o1.getVal ()]);
                }
                
                if (o2.getKind () == variable)
                {
                    o2 = Operand (SSALine, varTable [o2.getVal ()]);
                }
                
                /*if ( (r1.getKind () == variable || r1.getKind () == instruction) && r2.getKind () == constant)
                {
                    operand1 = r1.getSSAline ();
                    operand2 = r2.getVal ();
                }
                else if ( (r2.getKind () == variable || r2.getKind () == instruction) && r1.getKind () == constant)
                {
                    operand1 = r1.getVal ();
                    operand2 = r2.getSSAline ();
                }
                else
                {
                    operand1 = r1.getSSAline ();
                    operand2 = r2.getSSAline ();
                }*/
                //Instruction inst = Instruction (opc, o1/*perand1*/, o2/*perand2*/, line);
                currentBB->pushInstruction (new Instruction (opc, o1, o2, line) /*inst*/);
                o1 = Operand (SSALine, line);
                
                //r1 = Result (instruction, line);
            }
        }
        else if (tk == sub)
        {
            //rop = resultSub;
            lex.next ();                /** Consuming sub */
            Operand o2 = term ();
            
            if (o1.getKind () == constant && o2.getKind () == constant)
            {
                o1 = Operand (constant, o1.getVal () - o2.getVal ());
            }
            else
            {
                opCode opc = op_sub;
                //int operand1, operand2;
                int line = ++sp;
                if (o1.getKind () == variable)
                {
                    o1 = Operand (SSALine, varTable [o1.getVal ()]);
                }
                
                if (o2.getKind () == variable)
                {
                    o2 = Operand (SSALine, varTable [o2.getVal ()]);
                }
                
                /*if ( (r1.getKind () == variable || r1.getKind () == instruction) && r2.getKind () == constant)
                {
                    operand1 = r1.getSSAline ();
                    operand2 = r2.getVal ();
                }
                else if ( (r2.getKind () == variable || r2.getKind () == instruction) && r1.getKind () == constant)
                {
                    operand1 = r1.getVal ();
                    operand2 = r2.getSSAline ();
                }
                else
                {
                    operand1 = r1.getSSAline ();
                    operand2 = r2.getSSAline ();
                }*/
                //Instruction inst = Instruction (opc, o1/*perand1*/, o2/*perand2*/, line);
                currentBB->pushInstruction (new Instruction (opc, o1, o2, line) /*inst*/);
                o1 = Operand (SSALine, line);
                
                //r1 = Result (instruction, line);
            }
        }
        
        //Result r2 = term ();
        //Compute (rop, &r1, &r2);
        
        tk = lex.getToken ();
    }
    
    return o1;
}

Operand Parser::ident ()
{
	token tk = lex.getToken ();
	if (! (tk == identifier || tk == tk_read || tk == tk_write)) err ("ident");
    
    Operand res = Operand (variable, lex.getId ());
    
    if (tk == tk_read) res = Operand (type_read, -1);
    else if (tk == tk_write) res = Operand (type_write, -1);
    
    lex.next ();                        /** Consuming id */
    return res;
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

Operand Parser::term ()
{
	Operand o1 = factor ();
    //resultOp rop;

	token tk = lex.getToken ();
    
    while (tk == mul || tk == divis)
    {
        if (tk == mul)
        {
            //rop = resultMul;
            lex.next ();                /** Consuming mul */
            Operand o2 = factor ();
            
            if (o1.getKind () == constant && o2.getKind () == constant)
            {
                o1 = Operand (constant, o1.getVal () * o2.getVal ());
            }
            else
            {
                opCode opc = op_mul;
                //int operand1, operand2;
                int line = ++sp;
                if (o1.getKind () == variable)
                {
                    o1 = Operand (SSALine, varTable [o1.getVal ()]);
                }
                
                if (o2.getKind () == variable)
                {
                    o2 = Operand (SSALine, varTable [o2.getVal ()]);
                }
                
                /*if ( (r1.getKind () == variable || r1.getKind () == instruction) && r2.getKind () == constant)
                {
                    operand1 = r1.getSSAline ();
                    operand2 = r2.getVal ();
                }
                else if ( (r2.getKind () == variable || r2.getKind () == instruction) && r1.getKind () == constant)
                {
                    operand1 = r1.getVal ();
                    operand2 = r2.getSSAline ();
                }
                else
                {
                    operand1 = r1.getSSAline ();
                    operand2 = r2.getSSAline ();
                }*/
                //Instruction inst = Instruction (opc, o1/*perand1*/, o2/*perand2*/, line);
                currentBB->pushInstruction (new Instruction (opc, o1, o2, line)/*inst*/);
                o1 = Operand (SSALine, line);
                
                //r1 = Result (instruction, line);
            }
        }
        else if (tk == divis)
        {
            //rop = resultDiv;
            lex.next ();                /** Consuming divis */
            Operand o2 = factor ();
            
            if (o1.getKind () == constant && o2.getKind () == constant)
            {
                if (o2.getVal () == 0) err ("DIVISION BY ZERO");
                o1 = Operand (constant, o1.getVal () / o2.getVal ());
            }
            else
            {
                opCode opc = op_div;
                //int operand1, operand2;
                int line = ++sp;
                if (o1.getKind () == variable)
                {
                    o1 = Operand (SSALine, varTable [o1.getVal ()]);
                }
                
                if (o2.getKind () == variable)
                {
                    o2 = Operand (SSALine, varTable [o2.getVal ()]);
                }
                
                /*if (r1.getKind () == variable)
                {
                    operand1 = varTable [r1.getVal ()];
                }
                else if (r1.getKind () == instruction)
                {
                    operand1 = r1.getVal ();
                }
                else if (r1.getKind () == constant)
                {
                    operand1 = r1.getVal;
                }
                
                if (r2.getKind () == variable)
                {
                    operand2 = varTable [r2.getVal ()];
                }
                else if (r2.getKind () == instruction)
                {
                    operand2 = r2.getVal ();
                }
                else if (r2.getKind () == constant)
                {
                    operand2 = r2.getVal;
                }*/
                /*
                if ( (r1.getKind () == variable || r1.getKind () == instruction) && r2.getKind () == constant)
                {
                    operand1 = r1.getSSAline ();
                    operand2 = r2.getVal ();
                }
                else if ( (r2.getKind () == variable || r2.getKind () == instruction) && r1.getKind () == constant)
                {
                    operand1 = r1.getVal ();
                    operand2 = r2.getSSAline ();
                }
                else
                {
                    operand1 = r1.getSSAline ();
                    operand2 = r2.getSSAline ();
                }*/
                //Instruction inst = Instruction (opc, o1/*perand1*/, o2/*perand2*/, line);
                currentBB->pushInstruction (new Instruction (opc, o1, o2, line) /*inst*/);
                o1 = Operand (SSALine, line);
                
                //r1 = Result (instruction, line);
            }
        }
        
        //Result r2 = factor ();
        //Compute (rop, &r1, &r2);
        
        tk = lex.getToken ();
    }
    
    return o1;
}

Operand Parser::factor ()
{
    Operand o1;
    token tk = lex.getToken ();
    switch (tk)
    {
        case identifier:
        {
            o1 = designator ();
            
            break;
        }
        case num:
        {
            o1 = number ();
            
            break;
        }
        case openBracket:
        {
            lex.next ();                /** Consuming bracket */
            
            o1 = expression ();
            
            tk = lex.getToken ();
            if (tk != closeBracket) err ("factor");
            lex.next ();                /** Consuming bracket */
            break;
        }
        case tk_call:
        {
            o1 = funcCall ();
            //std::cout << "func call " << std::endl;
            //std::cout << sp << std::endl;
            break;
        }
        default:
        {
            err ("factor");
            break;
        }
    }
    
    return o1;
}

Operand Parser::number ()
{
	token tk = lex.getToken ();
	if (tk != num) err ("number");
    Operand res = Operand (constant, lex.getVal ());
    lex.next ();
    return res;
}

void Parser::err (std::string arg)
{
	std::cout << "Error in a Parser:" << std::endl;
    std::cout << "      " << arg << std::endl;
}
