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
        
        /* for (auto& t : varTable)
            std::cout << t.first << " " << t.second << std::endl;
        */
    }

}

void Parser::computation ()
{
    token tk = lex.getToken ();
	if (tk != tk_main) err ("computation_main");
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
	if (tk != openCurlBracket) err ("computation_opCurl");
    lex.next ();                    /** Consuming bracket */
    
	statSequence ();

	tk = lex.getToken ();
	if (tk != closeCurlBracket) err ("computation_clCurl");
    lex.next ();                    /** Consuming bracket */
    
	tk = lex.getToken ();
	if (tk != dot) err ("computation_dot");
    lex.next ();                    /** Consuming dot */
    
    tk = lex.getToken ();
    if (tk != eof) err ("computation_eof");          /** tk should be eof */
}

void Parser::varDecl ()
{
    typeDecl ();
    
    token tk = lex.getToken ();
    while (tk == identifier)
    {
        //Operand o = ident ();//
        
        varTable [lex.getId ()] = 0;
        lex.next ();                            /** Consuming identifier */
        /*
        if (o.getKind () == variable)
        {
            varTable [o.getVal ()] = 0; //Marking uninitialized variables with ssa line 0
        }
        else err ("Incorrect variable declaration");
          */
        
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
            
            int num = lex.getVal ();
            lex.next ();            /** Consuming array dimension */
            
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
    
    //ident ();
    tk = lex.getToken ();
    if (tk != identifier) err ("Incorrect function declaration!");
    int funcID = lex.getId ();
    lex.next ();                    /** Consuming function identifier */
    
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
        int varID = lex.getId ();   /** Consuming variable from the argument */
        lex.next ();
        //ident ();
        
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
        
        //std::cout << "statSequence token: " << tk << std::endl;
        if (tk == semicolon)
        {
            lex.next ();            /** Consuming semicolon */
            tk = lex.getToken ();
            //std::cout << "statSequence token: " << tk << std::endl;
        }
        
        //std::cout << "statSequence token: " << tk << std::endl;
    }
}

void Parser::statement ()
{
    token tk = lex.getToken ();
    
   // std::cout << "statement token: " << tk << std::endl;
    
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
    
	int o1 = designator ();
    //if (o1.getKind () != variable) err ("Incorrect assignment");
    //std::cout << "assignment designator: " << o1 << std::endl;

	token tk = lex.getToken ();
	if (tk != assign) err ("assignment");
    lex.next ();                    /** Consuming assign */
	
	int o2 = expression ();
   // std::cout << "assignment expression: " << o2 << std::endl;
    
    /*if (o2.getKind () == constant)
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
    else // o2 is SSA line*/
    //{
        if (o2 > 0)
        {
            varTable [o1] = o2;
        }
        else err ("Assigning non-existing SSA line");
    //}
    
    
}

int Parser::funcCall ()
{
    int result = 0;
    
    lex.next ();                    /** Consuming call */
    token tk = lex.getToken ();
    if (tk == tk_read)
    {
        lex.next ();                /** Consuming tk_read */
        lex.next ();                /** Consuming bracket */
        lex.next ();                /** Consuming bracket */
        
        int line = ++sp;
        currentBB->pushInstruction (new Instruction (op_read, -1, -1, line) );
        result = line;
        
    }
    else if (tk == tk_write)
    {
        lex.next ();                /** Consuming tk_write */
        lex.next ();                /** Consuming bracket */
        
        tk = lex.getToken ();
        int o1 = expression ();
        int line = ++sp;
        currentBB->pushInstruction (new Instruction (op_write, o1, -1, line) );
        result = line;
        
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
    
    return result;
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

int Parser::designator ()
{
    int result = 0;
	//int o1 = ident ();
    
    token tk = lex.getToken ();
    if (tk != identifier) err ("Incorrect identifier!");
    result = lex.getId ();
    lex.next ();                    /** Consuming identifier */
    
    tk = lex.getToken ();
    while (tk == openSqBracket)
    {
        lex.next ();                /** Consuming square bracket */
        
        expression ();              //@@@@  only static arrays?..
        
        tk = lex.getToken ();
        if (tk != closeSqBracket) err ("designator");
        lex.next ();                /** Consuming square bracket */
        tk = lex.getToken ();
    }
    
    return result;
}

int Parser::expression ()
{
    int result = 0;
    
	int o1 = term ();
    
    //std::cout << "expression term: " << o1 << std::endl;
    result = o1;
    
    //resultOp rop;

	token tk = lex.getToken ();
    //if (tk != add && tk != sub) err ("expression");
    while (tk == add || tk == sub)
    {
        opCode opc = op_add;
        if (tk == sub) opc = op_sub;
        lex.next ();                /** Consuming add/sub */
        int o2 = term ();
        
        /*if (o1.getKind () == constant && o2.getKind () == constant)
        {
            o1 = Operand (constant, o1.getVal () + o2.getVal ());
        }
        else*/
        
        int line = ++sp;
        currentBB->pushInstruction (new Instruction (opc, o1, o2, line) );
        result = line;
        
        /*if (tk == add)
        {
            //rop = resultAdd;
            lex.next ();
            int o2 = term ();
            
            
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
                
                //Instruction inst = Instruction (opc, o1, o2, line);
                currentBB->pushInstruction (new Instruction (opc, o1, o2, line) );
                o1 = Operand (SSALine, line);
                
                //r1 = Result (instruction, line);
            }
        }
        else if (tk == sub)
        {
            //rop = resultSub;
            lex.next ();
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
                
                //Instruction inst = Instruction (opc, o1, o2, line);
                currentBB->pushInstruction (new Instruction (opc, o1, o2, line) );
                o1 = Operand (SSALine, line);
                
                //r1 = Result (instruction, line);
            }
        }*/
        
        //Result r2 = term ();
        //Compute (rop, &r1, &r2);
        
        tk = lex.getToken ();
    }
    
    return result;
}

/*int Parser::ident ()
{
    int result = 0;
    
	token tk = lex.getToken ();
	if (! (tk == identifier || tk == tk_read || tk == tk_write)) err ("ident");
    
    Operand res = Operand (SSALine, varTable [lex.getId ()]); // do we have this variable
    
    if (tk == tk_read) res = Operand (type_read, -1);
    else if (tk == tk_write) res = Operand (type_write, -1);
    
    lex.next ();
    
    return result;
}*/

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

int Parser::term ()
{
    int result = 0;
    
	int o1 = factor ();
    result = o1;
    //std::cout << "term factor: " << o1 << std::endl;
    //resultOp rop;

	token tk = lex.getToken ();
    
    while (tk == mul || tk == divis)
    {
        opCode opc = op_mul;
        if (tk == sub) opc = op_div;
        lex.next ();                /** Consuming mul/div */
        int o2 = term ();
        
        /*if (o1.getKind () == constant && o2.getKind () == constant)
        {
            o1 = Operand (constant, o1.getVal () + o2.getVal ());
        }
        else*/
        
        int line = ++sp;
        currentBB->pushInstruction (new Instruction (opc, o1, o2, line) );
        result = line;
        /*
        if (tk == mul)
        {
            //rop = resultMul;
            lex.next ();
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
                
                
                //Instruction inst = Instruction (opc, o1, o2, line);
                currentBB->pushInstruction (new Instruction (opc, o1, o2, line));
                o1 = Operand (SSALine, line);
                
                //r1 = Result (instruction, line);
            }
        }
        else if (tk == divis)
        {
            //rop = resultDiv;
            lex.next ();
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
                
                
                //Instruction inst = Instruction (opc, o1, o2, line);
                currentBB->pushInstruction (new Instruction (opc, o1, o2, line) );
                o1 = Operand (SSALine, line);
                
                //r1 = Result (instruction, line);
            }
        }*/
        
        //Result r2 = factor ();
        //Compute (rop, &r1, &r2);
        
        tk = lex.getToken ();
    }
    
    return result;
}

int Parser::factor ()
{
    int result = 0;
    
    //Operand o1;
    token tk = lex.getToken ();
    switch (tk)
    {
        case identifier:
        {
            int o1 = designator ();
            result = varTable [o1];
            break;
        }
        case num:
        {
            //o1 = number ();
            int line = ++sp;
            currentBB->pushInstruction (new Instruction (op_const, lex.getVal (), -1, line) );
            result = line;
            lex.next ();                /** Consuming a number */
            break;
        }
        case openBracket:
        {
            lex.next ();                /** Consuming bracket */
            
            int o1 = expression ();
            
            tk = lex.getToken ();
            if (tk != closeBracket) err ("factor");
            lex.next ();                /** Consuming bracket */
            
            result = o1;
            break;
        }
        case tk_call:
        {
            int o1 = funcCall ();
            //std::cout << "func call " << std::endl;
            //std::cout << sp << std::endl;
            result = o1;
            break;
        }
        default:
        {
            err ("factor");
            break;
        }
    }
    
    return result;
}

/*Operand Parser::number ()
{
	token tk = lex.getToken ();
	if (tk != num) err ("number");
    Operand res = Operand (constant, lex.getVal ());
    lex.next ();
    return res;
}*/

void Parser::err (std::string arg)
{
	std::cout << "Error in a Parser:" << std::endl;
    std::cout << "      " << arg << std::endl;
}
