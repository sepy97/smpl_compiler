#include "Parser.h"

Parser::Parser (std::string s) : lex (s), CSE ()
{
	//lex = Lexer (s);
    sp = 0;
    bbCounter = 0;
    //std::cout << "BEFORE Module ctor" << std::endl;
    
    this->m = new Module ();
    
    //std::cout << "AFTER Module ctor" << std::endl;
}

void Parser::parse ()
{
    
    lex.next ();
    token tk = lex.getToken ();
    if (tk != tk_main) err ("Main function not found! ");
    else
    {
       // Instruction instr = new Instruction (++sp);
        BasicBlock* bb = new BasicBlock (++bbCounter);///*instr*/new Instruction (++sp));
        Function* f = new Function (std::string ("main"), bb);
        m->insertFunc (f);
        
        currentFunc = f;
        currentBB = bb;
        
        //std::cout << "BEFORE computation" << std::endl;
        
        computation ();
        
        //std::cout << "AFTER computation" << std::endl;
        //std::cout << m->body.size () << std::endl;
        
        std::cout << m->toString () << std::endl;
        
       // std::cout << "FINISHED!!!!" << std::endl;
        
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
	if (tk != openCurlBracket) err ("Syntax error: missing opening curling bracket!");
    lex.next ();                    /** Consuming bracket */
    
	statSequence ();

	tk = lex.getToken ();
	if (tk != closeCurlBracket) err ("Syntax error: missing closing curling bracket!");
    lex.next ();                    /** Consuming bracket */
    
	tk = lex.getToken ();
	if (tk != dot) err ("Syntax error: missing a dot at the end!");
    lex.next ();                    /** Consuming dot */
    
    tk = lex.getToken ();
    if (tk != eof) err ("computation_eof");          /** tk should be eof */
}

void Parser::varDecl ()
{
    /*bool isArray = */
    typeDecl ();
    
    token tk = lex.getToken ();
    if (isArray)
    {
        while (tk == identifier)
        {
            /*
            std::cout << lex.getId () << std::endl;
            std::cout << offset << std::endl;
            for (int i = 0; i < dims.size (); i++)
            {
                std::cout << dims[i] << " ";
            }
            std::cout << "\n";
            */
            
            arrTable [lex.getId ()] = std::make_pair (offset, dims);
            int shift = 1;
            for (int i = 0; i < dims.size (); i++)//auto &i: dims)
            {
                shift *= dims[i];//+1;
                //offset += dims[i]*SIZEOFINT;
            }
            offset += SIZEOFINT*shift;
            //arrTable.push_back (lex.getId (), dims);
            
            lex.next ();
            
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
                err ("Syntax error in array declaration!");
                break;
            }
        }
    }
    else
    {
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
                err ("Syntax error in variable declaration!");
                break;
            }
        }
    }
    
    dims.clear ();
    isArray = false;
    if (tk != semicolon) err ("Syntax error in variable declaration: missing semicolon!");
    lex.next ();                    /** Consuming semicolon */
}

void Parser::typeDecl ()
{
    token tk = lex.getToken ();
    if (tk == tk_var)
    {
        lex.next ();                /** Consuming var */
        isArray = false;
        //return false;
    }
    else if (tk == tk_array)
    {
        lex.next ();                /** Consuming array */
        tk = lex.getToken ();
        while (tk == openSqBracket)
        {
            //int dim = 0;
            lex.next ();            /** Consuming square bracket */
            tk = lex.getToken ();
            if (tk != num) err ("Syntax error in array declaration!");
            
            int num = lex.getVal ();
            lex.next ();            /** Consuming array dimension */
            dims.push_back (num);
            
            tk = lex.getToken ();
            if (tk != closeSqBracket) err ("Syntax error in array declaration: missing closing square bracket!");
            lex.next ();            /** Consuming square bracket */
            tk = lex.getToken ();
        }
        isArray = true;
        //return true;
    }
    //return false;
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
    if (tk != semicolon) err ("Syntax error in function declaration: missing semicolon!");
    lex.next ();                    /** Consuming semicolon */
    
    funcBody ();
    
    tk = lex.getToken ();
    if (tk != semicolon) err ("Syntax error in function declaration: missing semicolon!");
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
            err ("Syntax error: incorrect parameter description!");
            break;
        }
    }
    
    tk = lex.getToken ();
    if (tk != closeBracket) err ("Syntax error: missing closing bracket!");
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
    if (tk != openCurlBracket) err ("Syntax error: missing opening curling bracket!");
    lex.next ();                    /** Consuming curl bracket */
    tk = lex.getToken ();
    if (tk == tk_let || tk == tk_call || tk == tk_if || tk == tk_while || tk == tk_return)
    {
        statSequence ();
        
    }
    tk = lex.getToken ();
    if (tk != closeCurlBracket) err ("Syntax error: missing closing curling bracket!");
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
    int line = 0;
    bool needStore = false;
    lex.next ();                    /** Consuming let */
    
	int o1 = designator ();
    std::vector <int> storeIndexes;
    
    if (isArray)
    {
        needStore = true;
        storeIndexes = arrIndexes;
    }
    arrIndexes.clear ();
        

	token tk = lex.getToken ();
	if (tk != assign) err ("Syntax error in an assignment: missing equal sign!");
    lex.next ();                    /** Consuming assign */
    
    isArray = false;
    
	int o2 = expression ();
    //std::vector <int> loadIndexes;
    /*if (isArray)
    {
        loadIndexes = arrIndexes;
    }*/
    
    //std::cout << isArray << " " << o1 << " " << o2 << std::endl;
    
    /*
    if (isArray)
    {
        
        std::cout << ids.size () << " " << arrTable[result].second.size () << std::endl;
        for (int i = 0; i < ids.size (); i++)
        {
            std::cout << ids[i] << " ";
        }
        std::cout << "\n";
        
        if (ids.size () != arrTable [result].second.size ()) err ("Incorrect number of indices for the array!");
        
        int o1 = 0, o2 = 0;
        int line = ++sp;
        Instruction* instr = new Instruction (op_const, 4, -1, line);
        o1 = emitInstruction (instr);
        
        int idxLine = ids[0];
        int idsSize = ids.size ();
        for (int i = 1; i < idsSize; i++)
        {
            int line1 = ++sp;
            Instruction* constInstr = new Instruction (op_const, arrTable [result].second [i], -1, line1);
            int constLine = emitInstruction (constInstr);
            
            int mulLine = ++sp;
            //int mulres = 0;
            Instruction* mulInstr = new Instruction (op_mul, idxLine, constLine, mulLine);
            int mulres = emitInstruction (mulInstr);
            
            int addLine = ++sp;
            //int addres = 0;
            Instruction* addInstr = new Instruction (op_add, mulres, ids[i], addLine);
            int addres = emitInstruction (addInstr);
            
            idxLine = addres;
        }
        
        line = ++sp;
        Instruction* instr2 = new Instruction (op_mul, idxLine, o1, line);
        o1 = emitInstruction (instr2);
        
        line = ++sp;
        //int dimLine = 0;
        Instruction* dimInstr = new Instruction (op_const, arrTable [result].first, -1, line);
        int dimLine = emitInstruction (dimInstr);
        
        line = ++sp;
        Instruction* instr3 = new Instruction (op_add, -28, dimLine, line);
        o2 = emitInstruction (instr3);
        
        std::cout << "o1: " << o1 << " o2: " << o2 << std::endl;
        
        line = ++sp;
        Instruction* addaInstr = new Instruction (op_adda, o1, o2, line);
        result = emitInstruction (addaInstr);
    }*/
    
    if (isArray)
    {
        arrID1 = o2;
        o2 = emitLoad (o2);
        
        //std::cout << "emited load" << std::endl;
        
        /*
        //make load
        line = ++sp;
        
        
        //NEED CSE
        Instruction* instr = new Instruction (op_load, o2, -1, line);
        currentBB->pushInstruction (instr);
        o2 = line;
         */
        arrIndexes.clear ();
    }
   
    if (needStore)
    {
        arrIndexes = storeIndexes;
        arrID2 = o1;
        o1 = emitStore (o2, o1);
        
        //std::cout << "emited store" << std::endl;
        
        // insert store instruction (from o2 to o1)
        /*line = ++sp;
        
        //NEED CSE
        Instruction* instr = new Instruction (op_store, o2, o1, line);
        currentBB->pushInstruction (instr);
        */
        arrIndexes.clear ();
    }
    else
    {
        if (o2 > 0)
        {
            varTable [o1] = o2;
        }
        else if (o2 < 0)
        {
            varTable [o1] = varTable [-o2];
        }
        else err ("Assigning non-existing SSA line");
    }
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
        Instruction* instr = new Instruction (op_read, -1, -1, line);
        result = emitInstruction (instr);
        //@@@@ CSE
        /*
        int CSELine = findCommonSubexpression (instr);
        if (CSELine == 0)
        {
            currentBB->pushInstruction (instr);
            pushCSE (instr);
            result = line;
        } else result = CSELine;
        */
        //currentBB->pushInstruction (new Instruction (op_read, -1, -1, line) );
        //result = line;
        
    }
    else if (tk == tk_write)
    {
        lex.next ();                /** Consuming tk_write */
        lex.next ();                /** Consuming bracket */
        
        tk = lex.getToken ();
        int o1 = expression ();
        int line = 0;//++sp;
        
        int operand1 = o1;
        int var1 = -1;
        
        if (isArray)
        {
            
            //make load
            line = ++sp;
            
            //NEED CSE
            
            /*Instruction* instr = new Instruction (op_load, o1, -1, line);
            currentBB->pushInstruction (instr);*/
            arrID1 = o1;
            operand1 = emitLoad (o1);//line;
            
        }
        else
        {
            if (o1 < 0)
            {
                var1 = -o1;
                operand1 = varTable [var1];
            }
        }
        
        line = ++sp;
        Instruction* instr = new Instruction (op_write, operand1, -1, line);
        instr->setVar1 (var1);
        result = emitInstruction (instr);
        //@@@@ CSE
        /*
        int CSELine = findCommonSubexpression (instr);
        if (CSELine == 0)
        {
            currentBB->pushInstruction (instr);
            pushCSE (instr);
            result = line;
        } else result = CSELine;
        */
        //currentBB->pushInstruction (instr);
        //result = line;
        
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
            if (tk != closeBracket) err ("Syntax error: missing closing curling bracket!");
            lex.next ();                /** Consuming bracket */
        }
    }
    
    return result;
}

void Parser::ifStatement ()
{
    BasicBlock* thenEntryBB = new BasicBlock (++bbCounter);
    BasicBlock* elseEntryBB = new BasicBlock (++bbCounter);
    BasicBlock* fiBB        = new BasicBlock (++bbCounter);
    
    //if ()
    bool hasNestedBranch = false;
    
    currentJoinBB = fiBB;
    
    BasicBlock* ifBB = currentBB;
    
    lex.next ();                    /** Consuming if */

	relation ();
    
	token tk = lex.getToken ();
	if (tk != tk_then) err ("Syntax error in an if statement!");
    lex.next ();                    /** Consuming then */
	
    std::map <int, int> ifVarTable (varTable);          /** Making a local copy of varTable before all possible assignments in thenBB */
    
    currentBB = thenEntryBB;
    
    //Instruction* backupCSE [NUMOFOPS];
    std::array <Instruction*, NUMOFOPS> backupCSE = CSE;
    //std::copy (std::begin (CSE), std::end (CSE), backupCSE);
	
    statSequence ();
    
    BasicBlock* thenExitBB = currentBB;
    BasicBlock* elseExitBB;
    
    if (thenExitBB != thenEntryBB) hasNestedBranch = true;
    //bool nestedIf =
    int line = sp;
    
    std::map <int, int> thenVarTable (varTable);          /** Making a local copy of varTable after thenBB and before all possible assignments in elseBB */
    
    varTable = std::map <int, int> (ifVarTable);
    
    CSE = backupCSE;
    
	tk = lex.getToken ();
    bool wasElse = false;
	if (tk == tk_else)
	{
        wasElse = true;
        line = ++sp;
        
        //NEED CSE ??
        thenExitBB->pushInstruction (new Instruction (op_bra, -1 /* yet unknown */, -1, line));
        lex.next ();                /** Consuming else */
        
        currentBB = elseEntryBB;
		
        statSequence ();
        
        elseExitBB = currentBB;
        
        if (elseExitBB != elseEntryBB) hasNestedBranch = true;
    }
    
    std::map <int, int> elseVarTable (varTable);          /** Making a local copy of varTable after elseBB */
    
    CSE = backupCSE;
    
	tk = lex.getToken ();
	if (tk != tk_fi) err ("Syntax error in an if statement!");
    lex.next ();                    /** Consuming fi */
    
    //@@@@ add phi functions
    line = ++sp;
    
    //NEED CSE ??
    fiBB->pushInstruction (new Instruction (line));
    
    if (wasElse) ifThenElseDiamond (ifBB, thenEntryBB, elseEntryBB, thenExitBB, elseExitBB, fiBB, &ifVarTable, &thenVarTable, &elseVarTable);
    else ifThenDiamond (ifBB, thenEntryBB, thenExitBB, fiBB, &ifVarTable, &thenVarTable);
    
    //fiBB->body.pop_back ();    /** Removing nop instruction out of fiBB  */
    //sp--;
    
    currentBB = fiBB;
}

void Parser::whileStatement ()
{
    BasicBlock* whileBB = new BasicBlock (++bbCounter);
    BasicBlock* doBB    = new BasicBlock (++bbCounter);
    BasicBlock* odBB    = new BasicBlock (++bbCounter);
    
    BasicBlock* beforeBB = currentBB;
    
    lex.next ();                    /** Consuming while */
    
    currentBB = whileBB;
	relation ();

	token tk = lex.getToken ();
	if (tk != tk_do) err ("Syntax error in a while statement!");
    lex.next ();                    /** Consuming do */

    currentBB = doBB;
    std::map <int, int> beforeLoopVarTable (varTable);          /** Making a local copy of varTable before all possible assignments in doBB */
	
    statSequence ();
    
    BasicBlock* jmpBackBB = currentBB;
    int line = ++sp;
    
    //NEED CSE ??
    jmpBackBB->pushInstruction (new Instruction (op_bra, -1 /* yet unknown */, -1, line));
    //branch doesn't require CSE
    std::map <int, int> afterLoopVarTable (varTable);          /** Making a local copy of varTable after all assignments in jmpBackBB */

	tk = lex.getToken ();
	if (tk != tk_od) err ("Syntax error in a while statement!");
    lex.next ();                    /** Consuming od */
    
    //NEED CSE ??
    odBB->pushInstruction (new Instruction (++sp));
    
    whileDoDiamond (beforeBB, whileBB, doBB, jmpBackBB, odBB, &beforeLoopVarTable, &afterLoopVarTable);
    
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
    
    isArray = false;
    //int numOfDim = 0;
    //int index = 0;
    //std::vector<int> ids;
    while (tk == openSqBracket)
    {
        lex.next ();                /** Consuming square bracket */
        
        //int num = 0;                //@@@@  should be an id for each dimension
        int num = expression ();              //@@@@  only static arrays?..
        /*index *= dims [numOfDims];
        index += num;
        numOfDim++;*/
        arrIndexes.push_back (num);
        
        tk = lex.getToken ();
        if (tk != closeSqBracket) err ("Syntax error: missing closing square bracket!");
        lex.next ();                /** Consuming square bracket */
        tk = lex.getToken ();
        
        isArray = true;
    }
    /*
    if (isArray)
    {
        
        std::cout << ids.size () << " " << arrTable[result].second.size () << std::endl;
        for (int i = 0; i < ids.size (); i++)
        {
            std::cout << ids[i] << " ";
        }
        std::cout << "\n";
        
        if (ids.size () != arrTable [result].second.size ()) err ("Incorrect number of indices for the array!");
        
        int o1 = 0, o2 = 0;
        int line = ++sp;
        Instruction* instr = new Instruction (op_const, 4, -1, line);
        o1 = emitInstruction (instr);
        
        int idxLine = ids[0];
        int idsSize = ids.size ();
        for (int i = 1; i < idsSize; i++)
        {
            int line1 = ++sp;
            Instruction* constInstr = new Instruction (op_const, arrTable [result].second [i], -1, line1);
            int constLine = emitInstruction (constInstr);
            
            int mulLine = ++sp;
            //int mulres = 0;
            Instruction* mulInstr = new Instruction (op_mul, idxLine, constLine, mulLine);
            int mulres = emitInstruction (mulInstr);
            
            int addLine = ++sp;
            //int addres = 0;
            Instruction* addInstr = new Instruction (op_add, mulres, ids[i], addLine);
            int addres = emitInstruction (addInstr);
            
            idxLine = addres;
        }
        
        line = ++sp;
        Instruction* instr2 = new Instruction (op_mul, idxLine, o1, line);
        o1 = emitInstruction (instr2);
        
        line = ++sp;
        //int dimLine = 0;
        Instruction* dimInstr = new Instruction (op_const, arrTable [result].first, -1, line);
        int dimLine = emitInstruction (dimInstr);
        
        line = ++sp;
        Instruction* instr3 = new Instruction (op_add, -28, dimLine, line);
        o2 = emitInstruction (instr3);
        
        std::cout << "o1: " << o1 << " o2: " << o2 << std::endl;
        
        line = ++sp;
        Instruction* addaInstr = new Instruction (op_adda, o1, o2, line);
        result = emitInstruction (addaInstr);
    }*/
    //isArray = false;
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
        
        int var1 = -1, var2 = -1;
        int operand1 = result;
        if (result < 0)
        {
            var1 = -result;
            operand1 = varTable [var1];
        }
        int operand2 = o2;
        if (o2 < 0)
        {
            var2 = -o2;
            operand2 = varTable [var2];
        }
        
        Instruction* instr = new Instruction (opc, operand1, operand2, line);
        instr->setVar1 (var1);
        instr->setVar2 (var2);
        result = emitInstruction (instr);
        //@@@@ CSE
        /*
        int CSELine = findCommonSubexpression (instr);
        if (CSELine == 0)
        {
            currentBB->pushInstruction (instr);
            result = line;
            pushCSE (instr);
        } else result = CSELine;
        */
        //currentBB->pushInstruction (instr );
        //result = line;
        
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
    
    int var1 = -1, var2 = -1;
    int operand1 = o1;
    if (o1 < 0)
    {
        var1 = -o1;
        operand1 = varTable [var1];
    }
    int operand2 = o2;
    if (o2 < 0)
    {
        var2 = -o2;
        operand2 = varTable [var2];
    }
    
    Instruction* instr = new Instruction (op_cmp, operand1, operand2, line);
    instr->setVar1 (var1);
    instr->setVar2 (var2);
    int cmpLine = emitInstruction (instr);/*
    //@@@@ CSE
    int CSELine = findCommonSubexpression (instr);
    int cmpLine = 0;
    if (CSELine == 0)
    {
        currentBB->pushInstruction (instr);
        pushCSE (instr);
        cmpLine = line;
    } else cmpLine = CSELine;
    */
    
    //currentBB->pushInstruction (instr );
    line = ++sp;
    
    //NEED CSE ??
    currentBB->pushInstruction (new Instruction (opc, cmpLine, -1/*unknown SSALine*/, line));
    //No CSE for branching obviously
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
            err ("Syntax error: incorrect relation!");
            break;
        }
    }
}

int Parser::term ()
{
    int result = 0;
    
	int o1 = factor ();
    //if (o1 <)
    result = o1;
    //std::cout << "term factor: " << o1 << std::endl;
    //resultOp rop;

	token tk = lex.getToken ();
    
    while (tk == mul || tk == divis)
    {
        opCode opc = op_mul;
        if (tk == divis) opc = op_div;
        lex.next ();                /** Consuming mul/div */
        int o2 = term ();
        
        /*if (o1.getKind () == constant && o2.getKind () == constant)
        {
            o1 = Operand (constant, o1.getVal () + o2.getVal ());
        }
        else*/
        
        int line = ++sp;
        int var1 = -1, var2 = -1;
        int operand1 = result;
        if (result < 0)
        {
            var1 = -result;
            operand1 = varTable [var1];
        }
        int operand2 = o2;
        if (o2 < 0)
        {
            var2 = -o2;
            operand2 = varTable [var2];
        }
        
        Instruction* instr = new Instruction (opc, operand1, operand2, line);
        instr->setVar1 (var1);
        instr->setVar2 (var2);
        result = emitInstruction (instr);
        /*
        //@@@@ CSE
        int CSELine = findCommonSubexpression (instr);
        if (CSELine == 0)
        {
            currentBB->pushInstruction (instr);
            pushCSE (instr);
            result = line;
        } else result = CSELine;
        */
        
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
            int o1 = designator ();             /** negative variable identifier */
            if (isArray) result = o1;
            else result = -1*o1;
            //result = varTable [o1];
            break;
        }
        case num:
        {
            //o1 = number ();
            int line = ++sp;
            Instruction* instr = new Instruction (op_const, lex.getVal (), -1, line);
            result = emitInstruction (instr);/*
            //@@@@ CSE
            int CSELine = findCommonSubexpression (instr);
            if (CSELine == 0)
            {
                currentFunc->pushConstInstruction (instr);
                pushCSE (instr);
                result = line;
            } else result = CSELine;*/
            //currentFunc->pushConstInstruction (new Instruction (op_const, lex.getVal (), -1, line) );
            //currentBB->pushInstruction (new Instruction (op_const, lex.getVal (), -1, line) );  //@@@@ GENERATE a BASIC BLOCK for CONSTANTS
            //result = line;
            lex.next ();                /** Consuming a number */
            break;
        }
        case openBracket:
        {
            lex.next ();                /** Consuming bracket */
            
            int o1 = expression ();
            
            tk = lex.getToken ();
            if (tk != closeBracket) err ("Syntax error: missing closing bracket!");
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

void Parser::ifThenDiamond (BasicBlock* ifBB, BasicBlock* thenEntryBB, BasicBlock* thenExitBB, BasicBlock* fiBB, std::map <int, int>* ifVarTable, std::map <int, int>* thenVarTable)
{
    ifBB->successors.push_back     (thenEntryBB);
    thenEntryBB->predecessors.push_back (ifBB);
    
    ifBB->successors.push_back     (fiBB);
    fiBB->predecessors.push_back   (ifBB);
    
    thenExitBB->successors.push_back   (fiBB);
    fiBB->predecessors.push_back   (thenExitBB);
    
    //Instruction* thenExitBranch = thenExitBB->body.back ();
    //int fiAddr = fiBB->body.front ()->getLine ();
    //thenExitBranch->setOperand1 (fiAddr);
    
    /*
     @@@@
     ADD PHI instructions!
     */
    
    bool hasPhi = false;
    
    for (auto const& var: *ifVarTable)
    {
        if ( (*ifVarTable) [var.first] != (*thenVarTable) [var.first])
        {
            if ((*ifVarTable) [var.first] != 0 && (*thenVarTable) [var.first] != 0) /** Variable is initialized on both paths */
            {
                int line = ++sp;
                fiBB->phiInstructions.push_back ( std::pair<Instruction*, int> (new Instruction (op_phi, (*thenVarTable) [var.first], (*ifVarTable) [var.first], line), var.first));
                varTable [var.first] = line;
                
                hasPhi = true;
            }
        }
    }
    
    Instruction* ifBranch = ifBB->body.back ();
    int fiAddr = fiBB->body.front ()->getLine ();
    if (hasPhi) fiAddr = fiBB->phiInstructions.front ().first->getLine ();
    ifBranch->setOperand2 (fiAddr);
    
    
}

void Parser::ifThenElseDiamond (BasicBlock* ifBB, BasicBlock* thenEntryBB, BasicBlock* elseEntryBB, BasicBlock* thenExitBB, BasicBlock* elseExitBB, BasicBlock* fiBB, std::map <int, int>* ifVarTable, std::map <int, int>* thenVarTable, std::map <int, int>* elseVarTable)
{
    ifBB->successors.push_back     (thenEntryBB);
    thenEntryBB->predecessors.push_back (ifBB);
    
    ifBB->successors.push_back     (elseEntryBB);
    elseEntryBB->predecessors.push_back (ifBB);
    
    elseExitBB->successors.push_back   (fiBB);
    fiBB->predecessors.push_back   (elseExitBB);
    
    thenExitBB->successors.push_back   (fiBB);
    fiBB->predecessors.push_back   (thenExitBB);
    
    
    /*
     @@@@
     ADD PHI instructions!
     */
    
    for (auto const& var: *thenVarTable)
    {
        if ( (*thenVarTable) [var.first] != (*elseVarTable) [var.first])
        {
            if ((*elseVarTable) [var.first] != 0 && (*thenVarTable) [var.first] != 0) /** Variable is initialized on both paths */
            {
                int line = ++sp;
                fiBB->phiInstructions.push_back ( std::pair<Instruction*, int> (new Instruction (op_phi, (*thenVarTable) [var.first], (*elseVarTable) [var.first], line), var.first));
                varTable [var.first] = line;
            }
        }
    }
    
    
    Instruction* ifBranch = ifBB->body.back ();
    int elseEntryAddr = elseEntryBB->body.front ()->getLine ();
    if (! elseEntryBB->phiInstructions.empty ()) elseEntryAddr = elseEntryBB->phiInstructions.front ().first->getLine ();
    ifBranch->setOperand2 (elseEntryAddr);
    
    Instruction* thenExitBranch = thenExitBB->body.back ();
    int fiAddr = fiBB->body.front ()->getLine ();
    if (! fiBB->phiInstructions.empty ()) fiAddr = fiBB->phiInstructions.front ().first->getLine ();
    thenExitBranch->setOperand1 (fiAddr);
    
}

void Parser::whileDoDiamond (BasicBlock* beforeBB, BasicBlock* whileBB, BasicBlock* doBB, BasicBlock* jmpBackBB, BasicBlock* odBB, std::map <int, int>* beforeLoopVarTable, std::map <int, int>* afterLoopVarTable)
{
    beforeBB->successors.push_back  (whileBB);
    whileBB->predecessors.push_back (beforeBB);
    
    whileBB->successors.push_back   (doBB);
    doBB->predecessors.push_back    (whileBB);
    
    jmpBackBB->successors.push_back (whileBB);
    whileBB->predecessors.push_back (jmpBackBB);
    
   // jmpBackBB->successors.push_back (odBB);
   // odBB->predecessors.push_back    (jmpBackBB);
    
    whileBB->successors.push_back   (odBB);
    odBB->predecessors.push_back    (whileBB);
    
    
    /*
     @@@@
     ADD PHI instructions!
     */
    
    for (auto const& var: *beforeLoopVarTable)
    {
        if ( (*beforeLoopVarTable) [var.first] != (*afterLoopVarTable) [var.first])
        {
            if ((*beforeLoopVarTable) [var.first] != 0 && (*afterLoopVarTable) [var.first] != 0) /** Variable is initialized on both paths */
            {
                int line = ++sp;
                whileBB->phiInstructions.push_back ( std::pair<Instruction*, int> (new Instruction (op_phi, (*beforeLoopVarTable) [var.first], (*afterLoopVarTable) [var.first], line), var.first));
                varTable [var.first] = line;
                
                //std::cout << "PROPAGATING PHI!" << std::endl;
                
                //std::cout << beforeBB->toString () << std::endl;
                //std::cout << whileBB->toString () << std::endl;
                
                /*for (auto& t : varTable)
                    std::cout << t.first << " " << t.second << std::endl;
                */
                
                replaceWithPhi (whileBB, var.first, line);
                
                propagatePhi (/*whileBB*/doBB, jmpBackBB, var.first, line);
                
            }
        }
    }
    
    
    Instruction* jmpBackAddr = jmpBackBB->body.back ();
    int whileLine = whileBB->body.front ()->getLine ();
    if (! whileBB->phiInstructions.empty ()) whileLine = whileBB->phiInstructions.front ().first->getLine ();
    jmpBackAddr->setOperand1 (whileLine);// ->pushInstruction (new Instruction (op_bra, whileLine, -1, line));
    
    Instruction* whileBranch = whileBB->body.back ();
    int odLine = odBB->body.front ()->getLine ();
    if (! odBB->phiInstructions.empty ()) odLine = odBB->phiInstructions.front ().first->getLine ();
    whileBranch->setOperand2 (odLine);
}

void Parser::propagatePhi (BasicBlock* startBB, BasicBlock* endBB, int varID, int SSALine)
{
    //if startBB
    //std::cout << startBB->toString () << std::endl;
    
    //std::cout << endBB->toString () << std::endl;
    //std::cout << "@@@@@@@\n" << std::endl;
    //BasicBlock* currentBB = startBB;
    //while (true)
    {
        
        replaceWithPhi (startBB, varID, SSALine);
        
        if (startBB == endBB) return;
        
        if (startBB->successors.empty ()) return;
        for (auto& bb: startBB->successors)
        {
            
            propagatePhi (bb, endBB, varID, SSALine);
        }
        
    }
}

void Parser::replaceWithPhi (BasicBlock* bb, int varID, int SSALine)
{
    for (auto& p: bb->phiInstructions)
    {
        if (p.first->getVar1 () == varID)
        {
            p.first->setOperand1 (SSALine);
        }
        
        if (p.first->getVar2 () == varID)
        {
            p.first->setOperand2 (SSALine);
        }
    }
    
    for (auto& i: bb->body)
    {
        if (i->getVar1 () == varID)
        {
            i->setOperand1 (SSALine);
        }
    
        if (i->getVar2 () == varID)
        {
            i->setOperand2 (SSALine);
        }
    }
    
}

bool Parser::pushCSE (Instruction* instr)
{
    bool result = false;
    Instruction* before;
    opCode opc = nop;
    if (instr->getOp () == op_store)
    {
        opc = op_load;
        before = CSE [opc];
    }
    else
    {
        opc = instr->getOp ();
        before = CSE [opc];
    }
    if (before == nullptr) result = true;
    instr->setPrevDom (before);
    CSE [opc] = instr;
    return result;
}

Instruction* Parser::popCSE (opCode opc)
{
    //@@@@ NOT USING THIS FUNCTION
    Instruction* result = CSE [opc];
    Instruction* before = result->getPrevDom ();
    CSE [opc] = before;
    return result;
}

int Parser::findCommonSubexpression (Instruction* instr)
{
    int result = 0;
    opCode opc = instr->getOp ();
    if (opc == op_load)
    {
        Instruction* currentInstr = CSE [opc];
        while (currentInstr != nullptr )
        {
            //std::cout << "current: \nARR1: " << currentInstr->getArr1 () << " ARR2: " << currentInstr->getArr2 () << std::endl;
            //std::cout << "toCompare: \nARR1: " << instr->getArr1 () << " ARR2: " << instr->getArr2 () << std::endl;
            
            if (currentInstr->getOp () == op_store && currentInstr->getArr1 () == instr->getArr1 () && currentInstr->getArr2 () == instr->getArr2 ()) return 0;
            if (currentInstr->addaArg1 == instr->addaArg1 && currentInstr->addaArg2 == instr->addaArg2)
            {
                result = currentInstr->getLine ();
                return result;
            }/*
            if (currentInstr->compare (instr))
            {
                result = currentInstr->getLine ();
                return result;
            }*/
            //if (currentInstr)
            currentInstr = currentInstr->getPrevDom ();
        }
    }
    else
    {
        Instruction* currentInstr = CSE [opc];
        while (currentInstr != nullptr)
        {
            if (currentInstr->compare (instr))
            {
                result = currentInstr->getLine ();
                return result;
            }
            currentInstr = currentInstr->getPrevDom ();
        }
    }
    return result;
}

void Parser::dotGraph ()
{
    std::ofstream file;
    file.open ("CFG.dot");
    std::string basicBlocks = "", edges = "";
    
    m->dotGraph (&basicBlocks, &edges);
    
    file << basicBlocks;
    file << "\n\n";
    file << edges;
    
    file.close ();
}

int Parser::emitInstruction (Instruction* instr)
{
    int result = 0;
    
    switch (instr->getOp ())
    {
        case op_load:
        {
            currentBB->pushInstruction (instr);
            pushCSE (instr);
            result = instr->getLine ();
            break;
        }
        case op_adda:
        {
            currentBB->pushInstruction (instr);
            //pushCSE (instr);
            result = instr->getLine ();
            break;
        }
        case op_const:
        {
            int CSELine = findCommonSubexpression (instr);
            if (CSELine == 0)
            {
                currentFunc->pushConstInstruction (instr);
                pushCSE (instr);
                result = instr->getLine ();
            } else result = CSELine;
            break;
        }
        case op_store:
        {
            currentBB->pushInstruction (instr);
            pushCSE (instr);
            result = instr->getLine ();
            break;
        }
        //case op_load:
        /*{
            break;
        }*/
        //case op_adda:
        case op_neg:
        case op_add:
        case op_sub:
        case op_mul:
        case op_div:
        case op_cmp:
        {
            //std::cout << "STARTING CSE for \n" << instr->toString () << std::endl;
            int CSELine = findCommonSubexpression (instr);
            if (CSELine == 0)
            {
                currentBB->pushInstruction (instr);
                pushCSE (instr);
                result = instr->getLine ();
            } else result = CSELine;
            break;
        }
        default:
        {
            //std::cout << "NO CSE for \n" << instr->toString () << std::endl;
            currentBB->pushInstruction (instr);
            //pushCSE (instr);
            result = instr->getLine ();
            break;
        }
            
    }
    
    //std::cout << result << std::endl;
    return result;
}

int Parser::emitLoad (int toLoad)
{
    int result = 0;
    
    if (arrIndexes.size () != arrTable [toLoad].second.size ()) err ("Incorrect number of indexes for the array!");
    
    int o1 = 0, o2 = 0;
    int line = ++sp;
    Instruction* instr = new Instruction (op_const, 4, -1, line);
    o1 = emitInstruction (instr);
    
    int idxLine = arrIndexes[0];
    int idsSize = arrIndexes.size ();
    for (int i = 1; i < idsSize; i++)
    {
        int line1 = ++sp;
        Instruction* constInstr = new Instruction (op_const, arrTable [toLoad].second [i], -1, line1);
        int constLine = emitInstruction (constInstr);
        
        int mulLine = ++sp;
        //int mulres = 0;
        Instruction* mulInstr = new Instruction (op_mul, idxLine, constLine, mulLine);
        int mulres = emitInstruction (mulInstr);
        
        int addLine = ++sp;
        //int addres = 0;
        Instruction* addInstr = new Instruction (op_add, mulres, arrIndexes[i], addLine);
        int addres = emitInstruction (addInstr);
        
        idxLine = addres;
    }
    
    line = ++sp;
    Instruction* instr2 = new Instruction (op_mul, idxLine, o1, line);
    o1 = emitInstruction (instr2);
    
    line = ++sp;
    //int dimLine = 0;
    Instruction* dimInstr = new Instruction (op_const, arrTable [toLoad].first, -1, line);
    int dimLine = emitInstruction (dimInstr);
    
    line = ++sp;
    Instruction* instr3 = new Instruction (op_add, -28, dimLine, line);
    o2 = emitInstruction (instr3);
    
    line = ++sp;
    Instruction* addaInstr = new Instruction (op_adda, o1, o2, line);
    int loadArg = line;//emitInstruction (addaInstr);
    
    line = ++ sp;
    Instruction* loadInstr = new Instruction (op_load, loadArg, -1, line);
    
    loadInstr->addaArg1 = o1;
    loadInstr->addaArg2 = o2;
    
    loadInstr->setArr1 (arrID1);
    
    int CSELine = findCommonSubexpression (loadInstr);
    
    if (CSELine == 0)
    {
        emitInstruction (addaInstr);
        result = emitInstruction (loadInstr);
    }
    else
    {
        result = CSELine;
    }
    return result;
}

int Parser::emitStore (int what, int where)
{
    int result = 0;
    //std::cout << "what: " << what << " \n where: " << where << std::endl;
    
    /*std::cout << arrIndexes.size () << " : " << arrTable [where].second.size () << std::endl;
    for (int i = 0; i < arrIndexes.size (); i++)
    {
        std::cout << arrIndexes [i] << " ";
    }
    std::cout << "\n";
    */
    if (arrIndexes.size () != arrTable [where].second.size ()) err ("Incorrect number of indices for the array!");
    
    int o1 = 0, o2 = 0;
    int line = ++sp;
    Instruction* instr = new Instruction (op_const, 4, -1, line);
    o1 = emitInstruction (instr);
    
    int idxLine = arrIndexes[0];
    int idsSize = arrIndexes.size ();
    for (int i = 1; i < idsSize; i++)
    {
        int line1 = ++sp;
        Instruction* constInstr = new Instruction (op_const, arrTable [where].second [i], -1, line1);
        int constLine = emitInstruction (constInstr);
        
        int mulLine = ++sp;
        //int mulres = 0;
        Instruction* mulInstr = new Instruction (op_mul, idxLine, constLine, mulLine);
        int mulres = emitInstruction (mulInstr);
        
        int addLine = ++sp;
        //int addres = 0;
        Instruction* addInstr = new Instruction (op_add, mulres, arrIndexes[i], addLine);
        int addres = emitInstruction (addInstr);
        
        idxLine = addres;
    }
    
    line = ++sp;
    Instruction* instr2 = new Instruction (op_mul, idxLine, o1, line);
    o1 = emitInstruction (instr2);
    
    line = ++sp;
    //int dimLine = 0;
    Instruction* dimInstr = new Instruction (op_const, arrTable [where].first, -1, line);
    int dimLine = emitInstruction (dimInstr);
    
    line = ++sp;
    Instruction* instr3 = new Instruction (op_add, -28, dimLine, line);
    o2 = emitInstruction (instr3);
    
    line = ++sp;
    Instruction* addaInstr = new Instruction (op_adda, o1, o2, line);
    int storeArg = emitInstruction (addaInstr);
    
    line = ++sp;
    Instruction* storeInstr = new Instruction (op_store, what, storeArg, line);
    storeInstr->setArr1 (arrID1);
    storeInstr->setArr2 (arrID2);
    result = emitInstruction (storeInstr);
    
    return result;
}
