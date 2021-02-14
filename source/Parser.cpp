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
        varTable [lex.getId ()] = 0;
        lex.next ();                            /** Consuming identifier */
        
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

	token tk = lex.getToken ();
	if (tk != assign) err ("assignment");
    lex.next ();                    /** Consuming assign */
	int o2 = expression ();
   
    if (o2 > 0)
    {
        varTable [o1] = o2;
    }
    else err ("Assigning non-existing SSA line");
    
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
    BasicBlock* thenEntryBB = new BasicBlock ();
    BasicBlock* elseEntryBB = new BasicBlock ();
    BasicBlock* fiBB        = new BasicBlock ();
    
    BasicBlock* ifBB = currentBB;
    
    lex.next ();                    /** Consuming if */

	relation ();
    
	token tk = lex.getToken ();
	if (tk != tk_then) err ("Syntax error in an if statement!");
    lex.next ();                    /** Consuming then */
	
    currentBB = thenEntryBB;
	statSequence ();
    
    BasicBlock* thenExitBB = currentBB;
    BasicBlock* elseExitBB;
    
    int line = sp;
    
	tk = lex.getToken ();
    bool wasElse = false;
	if (tk == tk_else)
	{
        wasElse = true;
        line = ++sp;
        thenExitBB->pushInstruction (new Instruction (op_bra, -1 /* yet unknown */, -1, line));
        lex.next ();                /** Consuming else */
        
        currentBB = elseEntryBB;
		statSequence ();
        elseExitBB = currentBB;
    }
	
	tk = lex.getToken ();
	if (tk != tk_fi) err ("Syntax error in an if statement!");
    lex.next ();                    /** Consuming fi */
    
    //@@@@ add phi functions
    line = ++sp;
    fiBB->pushInstruction (new Instruction (line));
    
    if (wasElse) ifThenElseDiamond (ifBB, thenEntryBB, elseEntryBB, thenExitBB, elseExitBB, fiBB);
    else ifThenDiamond (ifBB, thenEntryBB, thenExitBB, fiBB);
    
    fiBB->body.pop_back ();    /** Removing nop instruction out of fiBB  */
    sp--;
    
    currentBB = fiBB;
}

void Parser::whileStatement ()
{
    BasicBlock* whileBB = new BasicBlock ();
    BasicBlock* doBB    = new BasicBlock ();
    BasicBlock* odBB    = new BasicBlock ();
    
    BasicBlock* beforeBB = currentBB;
    
    lex.next ();                    /** Consuming while */
    
    currentBB = whileBB;
	relation ();

	token tk = lex.getToken ();
	if (tk != tk_do) err ("Syntax error in a while statement!");
    lex.next ();                    /** Consuming do */

    currentBB = doBB;
	
    statSequence ();
    
    BasicBlock* jmpBackBB = currentBB;
    int line = ++sp;
    jmpBackBB->pushInstruction (new Instruction (op_bra, -1 /* yet unknown */, -1, line));

	tk = lex.getToken ();
	if (tk != tk_od) err ("Syntax error in a while statement!");
    lex.next ();                    /** Consuming od */
    
    //@@@@ add phi functions
    odBB->pushInstruction (new Instruction (++sp));
    
    whileDoDiamond (beforeBB, whileBB, doBB, jmpBackBB, odBB);
    
    odBB->body.pop_back ();    /** Removing nop instruction out of odBB  */
    sp--;
    
    currentBB = odBB;
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

	token tk = lex.getToken ();
    
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
        
        tk = lex.getToken ();
    }
    
    return result;
}

void Parser::relation ()
{
	int o1 = expression ();
    
    opCode opc = nop;
    token tk = lex.getToken ();
    switch (tk)
    {
        case eq:
        {
            opc = op_bne;
            lex.next ();                /** Consuming eq */
            break;
        }
        case neq:
        {
            opc = op_beq;
            lex.next ();                /** Consuming neq */
            break;
        }
        case less:
        {
            opc = op_bge;
            lex.next ();                /** Consuming less */
            break;
        }
        case le:
        {
            opc = op_bgt;
            lex.next ();                /** Consuming le */
            break;
        }
        case greater:
        {
            opc = op_ble;
            lex.next ();                /** Consuming greater */
            break;
        }
        case ge:
        {
            opc = op_blt;
            lex.next ();                /** Consuming ge */
            break;
        }
        default:
        {
            err ("Incorrect relation operation!");
            break;
        }
    }
    //relOp ();

	int o2 = expression ();
    
    int line = ++sp;
    currentBB->pushInstruction (new Instruction (op_cmp, o1, o2, line) );
    line = ++sp;
    currentBB->pushInstruction (new Instruction (opc, line-1, -1/*unknown SSALine*/, line));
    
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

void Parser::err (std::string arg)
{
	std::cout << "Error in a Parser:" << std::endl;
    std::cout << "      " << arg << std::endl;
}

void Parser::ifThenDiamond (BasicBlock* ifBB, BasicBlock* thenEntryBB, BasicBlock* thenExitBB, BasicBlock* fiBB)
{
    ifBB->successors.push_back     (thenEntryBB);
    thenEntryBB->predecessors.push_back (ifBB);
    
    ifBB->successors.push_back     (fiBB);
    fiBB->predecessors.push_back   (ifBB);
    
    thenExitBB->successors.push_back   (fiBB);
    fiBB->predecessors.push_back   (thenExitBB);
    
    Instruction* ifBranch = ifBB->body.back ();
    int fiAddr = fiBB->body.front ()->getLine ();
    ifBranch->setOperand2 (fiAddr);
    
    //Instruction* thenExitBranch = thenExitBB->body.back ();
    //int fiAddr = fiBB->body.front ()->getLine ();
    //thenExitBranch->setOperand1 (fiAddr);
    
    /*
     @@@@
     ADD PHI instructions!
     */
    
}

void Parser::ifThenElseDiamond (BasicBlock* ifBB, BasicBlock* thenEntryBB, BasicBlock* elseEntryBB, BasicBlock* thenExitBB, BasicBlock* elseExitBB, BasicBlock* fiBB)
{
    ifBB->successors.push_back     (thenEntryBB);
    thenEntryBB->predecessors.push_back (ifBB);
    
    ifBB->successors.push_back     (elseEntryBB);
    elseEntryBB->predecessors.push_back (ifBB);
    
    elseExitBB->successors.push_back   (fiBB);
    fiBB->predecessors.push_back   (elseExitBB);
    
    thenExitBB->successors.push_back   (fiBB);
    fiBB->predecessors.push_back   (thenExitBB);
    
    Instruction* ifBranch = ifBB->body.back ();
    int elseEntryAddr = elseEntryBB->body.front ()->getLine ();
    ifBranch->setOperand2 (elseEntryAddr);
    
    Instruction* thenExitBranch = thenExitBB->body.back ();
    int fiAddr = fiBB->body.front ()->getLine ();
    thenExitBranch->setOperand1 (fiAddr);
    
    /*
     @@@@
     ADD PHI instructions!
     */
    
}

void Parser::whileDoDiamond (BasicBlock* beforeBB, BasicBlock* whileBB, BasicBlock* doBB, BasicBlock* jmpBackBB, BasicBlock* odBB)
{
    beforeBB->successors.push_back  (whileBB);
    whileBB->predecessors.push_back (beforeBB);
    
    whileBB->successors.push_back   (doBB);
    doBB->predecessors.push_back    (whileBB);
    
    jmpBackBB->successors.push_back (whileBB);
    whileBB->predecessors.push_back (jmpBackBB);
    
    jmpBackBB->successors.push_back (odBB);
    odBB->predecessors.push_back    (jmpBackBB);
    
    whileBB->successors.push_back   (odBB);
    odBB->predecessors.push_back    (whileBB);
    
    //int line = ++sp;
    Instruction* jmpBackAddr = jmpBackBB->body.back ();
    int whileLine = whileBB->body.front ()->getLine ();
    jmpBackAddr->setOperand1 (whileLine);// ->pushInstruction (new Instruction (op_bra, whileLine, -1, line));
    
    Instruction* whileBranch = whileBB->body.back ();
    int odLine = odBB->body.front ()->getLine ();
    whileBranch->setOperand2 (odLine);
    
    /*
     @@@@
     ADD PHI instructions!
     */
}
