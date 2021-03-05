#include "Parser.h"
#define RELEASE 1

Parser::Parser (std::string s) : lex (s), CSE ()
{
    sp = 0;
    bbCounter = 0;
    
    this->m = new Module ();
}

void Parser::parse ()
{
    
    lex.next ();
    token tk = lex.getToken ();
    if (tk != tk_main) err ("Main function not found! ");
    else
    {
        BasicBlock* bb = new BasicBlock (++bbCounter);
        
        std::map <int, int> params;
        
        Function* f = new Function (0 /* main funcID */, params, bb);
        m->insertFunc (f);
        
        currentFunc = f;
        currentBB = bb;
        
        computation ();
        
        std::cout << m->toString () << std::endl;
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
    
	if (tk != openCurlBracket) err ("computation --- Syntax error: missing opening curling bracket!");
    lex.next ();                    /** Consuming bracket */
    
	statSequence ();

	tk = lex.getToken ();
	if (tk != closeCurlBracket) err ("computation --- Syntax error: missing closing curling bracket!");
    lex.next ();                    /** Consuming bracket */
    
	tk = lex.getToken ();
	if (tk != dot) err ("Syntax error: missing a dot at the end!");
    lex.next ();                    /** Consuming dot */
    
    tk = lex.getToken ();
    if (tk != eof) err ("computation_eof");          /** tk should be eof */
}

void Parser::varDecl ()
{
    typeDecl ();
    
    token tk = lex.getToken ();
    if (isArray)
    {
        while (tk == identifier)
        {
            currentFunc->arrTable [lex.getId ()] = std::make_pair (offset, dims);
            int shift = 1;
            for (int i = 0; i < dims.size (); i++)
            {
                shift *= dims[i];
            }
            offset += SIZEOFINT*shift;
            
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
            currentFunc->varTable [lex.getId ()] = 0;
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
    }
    else if (tk == tk_array)
    {
        lex.next ();                /** Consuming array */
        tk = lex.getToken ();
        while (tk == openSqBracket)
        {
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
    }
}

void Parser::funcDecl ()
{
    token tk = lex.getToken ();
    int isVoid = false;
    if (tk == tk_void)
    {
        lex.next ();                /** Consuming tk_void */
        isVoid = true;
        tk = lex.getToken ();
    }
    if (tk != tk_function) err ("Incorrect function declaration (missing 'function' keyword)!");
    lex.next ();                    /** Consuming tk_function */
    
    tk = lex.getToken ();
    if (tk != identifier) err ("Incorrect function declaration!");
    int funcID = lex.getId ();
    
    lex.next ();                    /** Consuming function identifier */
    
    std::vector <int> paramList = formalParam ();
    
    tk = lex.getToken ();
    if (tk != semicolon) err ("Syntax error in function declaration: missing semicolon after function header!");
    lex.next ();                    /** Consuming semicolon */
    
    Function* prevFunc = currentFunc;
    BasicBlock* prevBB = currentBB;
    BasicBlock* bb = new BasicBlock (++bbCounter);
    currentBB = bb;
    std::map <int, int> params;
    
    for (int i = 0; i < paramList.size (); i++)
    {
        params [paramList [i]] = -5;
    }
    Function* f = new Function (funcID, params, currentBB);
    f->isVoid = isVoid;
    
    funcTable [funcID] = f;
    
    m->insertFunc (f);
    currentFunc = f;
    
    funcBody ();
    
    currentFunc = prevFunc;
    currentBB = prevBB;
    
    tk = lex.getToken ();
    if (tk != semicolon) err ("Syntax error in function declaration: missing semicolon after function body!");
    lex.next ();                    /** Consuming semicolon */
}

std::vector <int> Parser::formalParam ()
{
    std::vector <int> result;
    
    token tk = lex.getToken ();
    if (tk != openBracket) err ("Incorrect function declaration: missing parameter brackets!");
    lex.next ();                    /** Consuming bracket */
    
    tk = lex.getToken ();
    while (tk == identifier)
    {
        int varID = lex.getId ();   /** Consuming variable from the argument */
        result.push_back (varID);
        lex.next ();
        
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
    
    return result;
}

void Parser::funcBody ()
{
    token tk = lex.getToken ();
    while (tk == tk_var || tk == tk_array)
    {
        varDecl ();
        
        tk = lex.getToken ();
    }
    
    int numOfParams = 0;
    for (auto& x : currentFunc->params)
    {
        Instruction* instr = new Instruction (op_mu, ++numOfParams /*x.first*/, -1, ++sp);
        int ssaLine = emitInstruction (instr);
        currentFunc->varTable [x.first] = ssaLine;
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
			funcCall (true);
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
    
    if (isArray)
    {
        arrID1 = o2;
        o2 = emitLoad (o2);
        
        arrIndexes.clear ();
    }
   
    if (needStore)
    {
        arrIndexes = storeIndexes;
        arrID2 = o1;
        o1 = emitStore (o2, o1);
        
        arrIndexes.clear ();
    }
    else
    {
        if (o2 > 0)
        {
            int varID = o1;
            int searchingLine = o2;
            Instruction* instr;
            
            for (int j = allInstructions.size (); --j >= 0;)
            {
                if (allInstructions [j]->getLine () == searchingLine)
                {
                    instr = allInstructions [j];
                }
            }
            
            if (std::find (instr->referredVariables.begin (), instr->referredVariables.end (), varID) == instr->referredVariables.end ())
            {
                instr->referredVariables.push_back (varID);
            }
            currentFunc->varTable [o1] = o2;
        }
        else if (o2 < 0)
        {
            int varID = o1;
            int searchingLine = currentFunc->varTable [-o2];
            if (searchingLine == 0) err ("Assignment of a non-initialized variable!");
            Instruction* instr;
            
            for (int j = allInstructions.size (); --j >= 0;)
            {
                if (allInstructions [j]->getLine () == searchingLine)
                {
                    instr = allInstructions [j];
                }
            }
            
            if (std::find (instr->referredVariables.begin (), instr->referredVariables.end (), varID) == instr->referredVariables.end ())
            {
                instr->referredVariables.push_back (varID);
            }
            currentFunc->varTable [o1] = currentFunc->varTable [-o2];
        }
        else err ("Assigning non-existing SSA line");
    }
}

int Parser::funcCall (bool isVoid)
{
    int result = 0;
    
    lex.next ();                    /** Consuming call */
    token tk = lex.getToken ();
    if (tk == tk_read)
    {
        if (isVoid) err ("Incorrect call of a non-void function!");
        
        lex.next ();                /** Consuming tk_read */
        tk = lex.getToken ();
        if (tk == openBracket)
        {
            lex.next ();                /** Consuming opening bracket */
            lex.next ();                /** Consuming closing bracket */
        }
        
        int line = ++sp;
        Instruction* instr = new Instruction (op_read, -1, -1, line);
        result = emitInstruction (instr);
        
    }
    else if (tk == tk_write)
    {
        
        if (!isVoid) err ("Incorrect call of a void function!");
        
        lex.next ();                /** Consuming tk_write */
        lex.next ();                /** Consuming bracket */
        tk = lex.getToken ();
        int o1 = expression ();
        int line = 0;
        int operand1 = o1;
        int var1 = -1;
        
        if (isArray)
        {
            line = ++sp;
            arrID1 = o1;
            operand1 = emitLoad (o1);
        }
        else
        {
            if (o1 < 0)
            {
                var1 = -o1;
                operand1 = currentFunc->varTable [var1];
                if (operand1 == 0) err ("Writing a non-initialized variable!");
            }
        }
        
        line = ++sp;
        Instruction* instr = new Instruction (op_write, operand1, -1, line);
        instr->setVar1 (var1);
        result = emitInstruction (instr);
        lex.next ();                /** Consuming bracket */
    }
    else
    {
        int line = 0;
        std::vector <int> funcArgs;
        int funcID = lex.getId ();
        
        auto funcIterator = funcTable.find (funcID);
        if (funcIterator == funcTable.end()) err ("Call of undeclared function!");
        if (funcIterator->second->isVoid == true && isVoid == false)
        {
            err ("Incorrect call of a void function!");
        }
        else if (funcIterator->second->isVoid == false && isVoid == true)
        {
            err ("Incorrect call of a non-void function!");
        }
        
        lex.next ();
        token tk = lex.getToken ();
        if (tk == openBracket)
        {
            lex.next ();                /** Consuming bracket */
            tk = lex.getToken ();
            while (tk == identifier || tk == num || tk == openBracket || tk == tk_call)
            {
                int paramLine = expression ();
                if (paramLine < 0) paramLine = currentFunc->varTable [-paramLine];
                funcArgs.push_back (paramLine);
                
                tk = lex.getToken ();
                if (tk == comma)
                {
                    lex.next ();        /** Consuming comma */
                    tk = lex.getToken ();
                }
            }
            tk = lex.getToken ();
            if (tk != closeBracket) err ("Syntax error: missing closing bracket!");
            lex.next ();                /** Consuming bracket */
        }
        
        Function* f = funcTable [funcID];
        if (f->params.size () != funcArgs.size ()) err ("Incorrect number of arguments in a function call!");
        line = ++sp;
        Instruction* instr = new Instruction (op_call, f->getEntry ()->label, -1, line);
        int paramIncrement = 0;
        for (auto& x : f->params)
        {
            x.second = funcArgs [paramIncrement];
            f->varTable [x.first] = funcArgs [paramIncrement];
            instr->funcArgs.push_back (funcArgs [paramIncrement]);
            paramIncrement++;
        }
        result = emitInstruction (instr);
    }
    
    return result;
}

void Parser::ifStatement ()
{
    BasicBlock* thenEntryBB = new BasicBlock (++bbCounter);
    BasicBlock* elseEntryBB = new BasicBlock (++bbCounter);
    BasicBlock* fiBB        = new BasicBlock (++bbCounter);
    
    bool hasNestedBranch = false;
    BasicBlock* ifBB = currentBB;
    lex.next ();                    /** Consuming if */
	
    relation ();
    
	token tk = lex.getToken ();
	if (tk != tk_then) err ("Syntax error in an if statement!");
    lex.next ();                    /** Consuming then */
	
    std::map <int, int> ifVarTable (currentFunc->varTable);          /** Making a local copy of varTable before all possible assignments in thenBB */
    currentBB = thenEntryBB;
    std::array <Instruction*, NUMOFOPS> backupCSE = CSE;
	
    statSequence ();
    
    BasicBlock* thenExitBB = currentBB;
    BasicBlock* elseExitBB;
    if (thenExitBB != thenEntryBB) hasNestedBranch = true;
    int line = sp;
    std::map <int, int> thenVarTable (currentFunc->varTable);          /** Making a local copy of varTable after thenBB and before all possible assignments in elseBB */
    currentFunc->varTable = std::map <int, int> (ifVarTable);
    CSE = backupCSE;
	tk = lex.getToken ();
    bool wasElse = false;
	if (tk == tk_else)
	{
        wasElse = true;
        line = ++sp;
        Instruction* thenInstr = new Instruction (op_bra, -1 /* yet unknown */, -1, line);
        thenExitBB->pushInstruction (thenInstr);
        allInstructions.push_back (thenInstr);
        lex.next ();                /** Consuming else */
        currentBB = elseEntryBB;
		
        statSequence ();
        
        elseExitBB = currentBB;
        if (elseExitBB != elseEntryBB) hasNestedBranch = true;
    }
    
    std::map <int, int> elseVarTable (currentFunc->varTable);          /** Making a local copy of varTable after elseBB */
    CSE = backupCSE;
    tk = lex.getToken ();
	if (tk != tk_fi) err ("Syntax error in an if statement!");
    lex.next ();                    /** Consuming fi */
    line = ++sp;
    Instruction* fiInstr = new Instruction (line);
    fiBB->pushInstruction (fiInstr);
    allInstructions.push_back (fiInstr);
    
    if (wasElse) ifThenElseDiamond (ifBB, thenEntryBB, elseEntryBB, thenExitBB, elseExitBB, fiBB, &ifVarTable, &thenVarTable, &elseVarTable);
    else ifThenDiamond (ifBB, thenEntryBB, thenExitBB, fiBB, &ifVarTable, &thenVarTable);
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
    std::map <int, int> beforeLoopVarTable (currentFunc->varTable);          /** Making a local copy of varTable before all possible assignments in doBB */
	
    statSequence ();
    
    BasicBlock* jmpBackBB = currentBB;
    int line = ++sp;
    Instruction* braInstr = new Instruction (op_bra, -1 /* yet unknown */, -1, line);
    jmpBackBB->pushInstruction (braInstr);
    allInstructions.push_back (braInstr);
    //branch doesn't require CSE
    std::map <int, int> afterLoopVarTable (currentFunc->varTable);          /** Making a local copy of varTable after all assignments in jmpBackBB */
    tk = lex.getToken ();
	if (tk != tk_od) err ("Syntax error in a while statement!");
    lex.next ();                    /** Consuming od */
    Instruction* odInstr = new Instruction (++sp);
    odBB->pushInstruction (odInstr);
    allInstructions.push_back (odInstr);
    
    whileDoDiamond (beforeBB, whileBB, doBB, jmpBackBB, odBB, &beforeLoopVarTable, &afterLoopVarTable);
    currentBB = odBB;
}

void Parser::returnStatement ()
{
    lex.next ();                    /** Consuming return */
    token tk = lex.getToken ();
    if (tk == identifier || tk == num || tk == openBracket || tk == tk_call)
    {
        int o1 = expression ();
        if (o1 > 0) currentFunc->returningValue = o1;
        else if (o1 < 0) currentFunc->returningValue = currentFunc->varTable [-o1];
        Instruction* retInstr = new Instruction (op_ret, currentFunc->returningValue, -1, ++sp);
        emitInstruction (retInstr);
    }
}

int Parser::designator ()
{
    int result = 0;
    token tk = lex.getToken ();
    if (tk != identifier) err ("Incorrect identifier!");
    result = lex.getId ();
    lex.next ();                    /** Consuming identifier */
    tk = lex.getToken ();
    isArray = false;
    while (tk == openSqBracket)
    {
        lex.next ();                /** Consuming square bracket */
        
        int num = expression ();
        
        arrIndexes.push_back (num);
        tk = lex.getToken ();
        if (tk != closeSqBracket) err ("Syntax error: missing closing square bracket!");
        lex.next ();                /** Consuming square bracket */
        tk = lex.getToken ();
        isArray = true;
    }
    return result;
}

int Parser::expression ()
{
    int result = 0;
    
	int o1 = term ();
    
    result = o1;
	token tk = lex.getToken ();
    while (tk == add || tk == sub)
    {
        opCode opc = op_add;
        if (tk == sub) opc = op_sub;
        lex.next ();                /** Consuming add/sub */
        
        int o2 = term ();
        
        int line = ++sp;
        int var1 = -1, var2 = -1;
        int operand1 = result;
        if (result < 0)
        {
            var1 = -result;
            operand1 = currentFunc->varTable [var1];
        }
        int operand2 = o2;
        if (o2 < 0)
        {
            var2 = -o2;
            operand2 = currentFunc->varTable [var2];
        }
        Instruction* instr = new Instruction (opc, operand1, operand2, line);
        instr->setVar1 (var1);
        instr->setVar2 (var2);
        result = emitInstruction (instr);
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

	int o2 = expression ();
    
    int line = ++sp;
    int var1 = -1, var2 = -1;
    int operand1 = o1;
    if (o1 < 0)
    {
        var1 = -o1;
        operand1 = currentFunc->varTable [var1];
    }
    int operand2 = o2;
    if (o2 < 0)
    {
        var2 = -o2;
        operand2 = currentFunc->varTable [var2];
    }
    
    Instruction* instr = new Instruction (op_cmp, operand1, operand2, line);
    instr->setVar1 (var1);
    instr->setVar2 (var2);
    int cmpLine = emitInstruction (instr);
    line = ++sp;
    Instruction* cmpInstr = new Instruction (opc, cmpLine, -1 /*unknown SSALine*/, line);
    currentBB->pushInstruction (cmpInstr);
    allInstructions.push_back (cmpInstr);
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
    
    result = o1;
	token tk = lex.getToken ();
    while (tk == mul || tk == divis)
    {
        opCode opc = op_mul;
        if (tk == divis) opc = op_div;
        lex.next ();                /** Consuming mul/div */
        
        int o2 = factor (); //@@@@ factor instead of term!!!
        
        int line = ++sp;
        int var1 = -1, var2 = -1;
        int operand1 = result;
        if (result < 0)
        {
            var1 = -result;
            operand1 = currentFunc->varTable [var1];
        }
        int operand2 = o2;
        if (o2 < 0)
        {
            var2 = -o2;
            operand2 = currentFunc->varTable [var2];
        }
        Instruction* instr = new Instruction (opc, operand1, operand2, line);
        instr->setVar1 (var1);
        instr->setVar2 (var2);
        result = emitInstruction (instr);
        tk = lex.getToken ();
    }
    
    return result;
}

int Parser::factor ()
{
    int result = 0;
    token tk = lex.getToken ();
    switch (tk)
    {
        case identifier:
        {
            int o1 = designator ();             /** negative variable identifier */
            
            if (isArray) result = o1;
            else result = -1*o1;
            break;
        }
        case num:
        {
            int line = ++sp;
            Instruction* instr = new Instruction (op_const, lex.getVal (), -1, line);
            result = emitInstruction (instr);
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
            int o1 = funcCall (false);
            
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
    std::cout << "     ------------- " << arg << std::endl;
    
    //@@@@
    // kill the app
#ifdef RELEASE
    exit (0);
#endif
}

void Parser::ifThenDiamond (BasicBlock* ifBB, BasicBlock* thenEntryBB, BasicBlock* thenExitBB, BasicBlock* fiBB, std::map <int, int>* ifVarTable, std::map <int, int>* thenVarTable)
{
    ifBB->successors.push_back          (thenEntryBB);
    thenEntryBB->predecessors.push_back (ifBB);
    ifBB->successors.push_back          (fiBB);
    fiBB->predecessors.push_back        (ifBB);
    thenExitBB->successors.push_back    (fiBB);
    fiBB->predecessors.push_back        (thenExitBB);
    
    bool hasPhi = false;
    for (auto const& var: *ifVarTable)
    {
        if ( (*ifVarTable) [var.first] != (*thenVarTable) [var.first])
        {
            if ((*ifVarTable) [var.first] != 0 && (*thenVarTable) [var.first] != 0) /** Variable is initialized on both paths */
            {
                int line = ++sp;
                Instruction* instr = new Instruction (op_phi, (*thenVarTable) [var.first], (*ifVarTable) [var.first], line);
                instr->setVar1 (var.first);
                instr->setVar2 (var.first);
                fiBB->phiInstructions.push_back ( std::pair<Instruction*, int> (instr, var.first));
                currentFunc->varTable [var.first] = line;
                allInstructions.push_back (instr);
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
    ifBB->successors.push_back          (thenEntryBB);
    thenEntryBB->predecessors.push_back (ifBB);
    ifBB->successors.push_back          (elseEntryBB);
    elseEntryBB->predecessors.push_back (ifBB);
    elseExitBB->successors.push_back    (fiBB);
    fiBB->predecessors.push_back        (elseExitBB);
    thenExitBB->successors.push_back    (fiBB);
    fiBB->predecessors.push_back        (thenExitBB);
    
    for (auto const& var: *thenVarTable)
    {
        if ( (*thenVarTable) [var.first] != (*elseVarTable) [var.first])
        {
            if ((*elseVarTable) [var.first] != 0 && (*thenVarTable) [var.first] != 0) /** Variable is initialized on both paths */
            {
                int line = ++sp;
                Instruction* instr = new Instruction (op_phi, (*thenVarTable) [var.first], (*elseVarTable) [var.first], line);
                instr->setVar1 (var.first);
                instr->setVar2 (var.first);
                fiBB->phiInstructions.push_back ( std::pair<Instruction*, int> (instr, var.first));
                currentFunc->varTable [var.first] = line;
                allInstructions.push_back (instr);
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
    whileBB->successors.push_back   (odBB);
    odBB->predecessors.push_back    (whileBB);
    
    for (auto const& var: *beforeLoopVarTable)
    {
        if ( (*beforeLoopVarTable) [var.first] != (*afterLoopVarTable) [var.first])
        {
            if ((*beforeLoopVarTable) [var.first] != 0 && (*afterLoopVarTable) [var.first] != 0) /** Variable is initialized on both paths */
            {
                int line = ++sp;
                Instruction* instr = new Instruction (op_phi, (*beforeLoopVarTable) [var.first], (*afterLoopVarTable) [var.first], line);
                instr->setVar1 (var.first);
                instr->setVar2 (var.first);
                whileBB->phiInstructions.push_back ( std::pair<Instruction*, int> (instr, var.first));
                allInstructions.push_back (instr);
                currentFunc->varTable [var.first] = line;
                int varID = var.first;
                if (std::find (instr->referredVariables.begin (), instr->referredVariables.end (), varID) == instr->referredVariables.end ())
                {
                    instr->referredVariables.push_back (varID);
                }
                visitedBB.clear ();
                bool afterLine = false;
                for (auto& p: whileBB->phiInstructions)
                {
                    if (afterLine)
                    {
                        if (p.first->getVar1 () == varID) p.first->setOperand1 (line);
                        if (p.first->getVar2 () == varID) p.first->setOperand2 (line);
                        if (find (p.first->referredVariables.begin (), p.first->referredVariables.end (), varID) != p.first->referredVariables.end ()) break;
                    }
                    else
                    {
                        if (p.first->getLine () == line) afterLine = true;
                    }
                }
                for (auto& i: whileBB->body)
                {
                    if (i->getVar1 () == varID) i->setOperand1 (line);
                    if (i->getVar2 () == varID) i->setOperand2 (line);
                    if (find (i->referredVariables.begin (), i->referredVariables.end (), varID) != i->referredVariables.end ()) break;
                }
                propagatePhi (doBB, jmpBackBB, var.first, line);
            }
        }
    }
    Instruction* jmpBackAddr = jmpBackBB->body.back ();
    int whileLine = whileBB->body.front ()->getLine ();
    if (! whileBB->phiInstructions.empty ()) whileLine = whileBB->phiInstructions.front ().first->getLine ();
    jmpBackAddr->setOperand1 (whileLine);
    Instruction* whileBranch = whileBB->body.back ();
    int odLine = odBB->body.front ()->getLine ();
    if (! odBB->phiInstructions.empty ()) odLine = odBB->phiInstructions.front ().first->getLine ();
    whileBranch->setOperand2 (odLine);
}

bool Parser::propagatePhi (BasicBlock* startBB, BasicBlock* endBB, int varID, int SSALine)
{
    BasicBlock* processedBB = startBB;
    bool isFinished = false;
    while (true)
    {
        isFinished = replaceWithPhi (processedBB, varID, SSALine);
        if (processedBB->successors.empty ()) return isFinished;
        if (processedBB == endBB)             return isFinished;
        if (isFinished)                       return true;
        
        if (processedBB->successors.size () > 1)
        {
            for (auto& bb: processedBB->successors)
            {
                if ((visitedBB.insert (processedBB)).second == true ) isFinished = propagatePhi (bb, endBB, varID, SSALine);
            }
            return isFinished;
        }
        else processedBB = processedBB->successors.front ();
    }
    return isFinished;
}

bool Parser::replaceWithPhi (BasicBlock* bb, int varID, int SSALine)
{
    bool isFinished = false;
    
    if (bb->body.empty () && bb->phiInstructions.empty ()) return isFinished;
    
    for (auto& p: bb->phiInstructions)
    {
        if (p.first->getVar1 () == varID) p.first->setOperand1 (SSALine);
        
        if (find (p.first->referredVariables.begin (), p.first->referredVariables.end (), varID) != p.first->referredVariables.end ()) return true;
    }
    
    for (auto& i: bb->body)
    {
        if (i->getVar1 () == varID) i->setOperand1 (SSALine);
        if (i->getVar2 () == varID) i->setOperand2 (SSALine);
        if (find (i->referredVariables.begin (), i->referredVariables.end (), varID) != i->referredVariables.end ()) return true;
    }
    return isFinished;
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

/*
Instruction* Parser::popCSE (opCode opc)
{
    //@@@@ NOT USING THIS FUNCTION
    Instruction* result = CSE [opc];
    Instruction* before = result->getPrevDom ();
    CSE [opc] = before;
    return result;
}*/

int Parser::findCommonSubexpression (Instruction* instr)
{
    int result = 0;
    opCode opc = instr->getOp ();
    if (opc == op_load)
    {
        Instruction* currentInstr = CSE [opc];
        while (currentInstr != nullptr )
        {
            if (currentInstr->getOp () == op_store && currentInstr->getArr1 () == instr->getArr1 () && currentInstr->getArr2 () == instr->getArr2 ()) return 0;
            if (currentInstr->addaArg1 == instr->addaArg1 && currentInstr->addaArg2 == instr->addaArg2) return currentInstr->getLine ();
            currentInstr = currentInstr->getPrevDom ();
        }
    }
    else
    {
        Instruction* currentInstr = CSE [opc];
        while (currentInstr != nullptr)
        {
            if (currentInstr->compare (instr)) return currentInstr->getLine ();
            currentInstr = currentInstr->getPrevDom ();
        }
    }
    return result;
}

void Parser::dotGraph (std::string compiledIR)
{
    std::ofstream file;
    file.open (compiledIR);
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
            allInstructions.push_back (instr);
            pushCSE (instr);
            result = instr->getLine ();
            break;
        }
        case op_adda:
        {
            currentBB->pushInstruction (instr);
            allInstructions.push_back (instr);
            result = instr->getLine ();
            break;
        }
        case op_const:
        {
            int CSELine = findCommonSubexpression (instr);
            if (CSELine == 0)
            {
                currentFunc->pushConstInstruction (instr);
                allInstructions.push_back (instr);
                pushCSE (instr);
                result = instr->getLine ();
            } else result = CSELine;
            break;
        }
        case op_store:
        {
            currentBB->pushInstruction (instr);
            allInstructions.push_back (instr);
            pushCSE (instr);
            result = instr->getLine ();
            break;
        }
        case op_neg:
        case op_add:
        case op_sub:
        case op_mul:
        case op_div:
        case op_cmp:
        {
            int CSELine = findCommonSubexpression (instr);
            if (CSELine == 0)
            {
                currentBB->pushInstruction (instr);
                allInstructions.push_back (instr);
                pushCSE (instr);
                result = instr->getLine ();
            } else result = CSELine;
            break;
        }
        default:
        {
            currentBB->pushInstruction (instr);
            allInstructions.push_back (instr);
            result = instr->getLine ();
            break;
        }
    }
    return result;
}

int Parser::emitLoad (int toLoad)
{
    int result = 0;
    if (arrIndexes.size () != currentFunc->arrTable [toLoad].second.size ()) err ("Incorrect number of indexes for the array!");
    int o1 = 0, o2 = 0;
    int line = ++sp;
    Instruction* instr = new Instruction (op_const, SIZEOFINT, -1, line);
    o1 = emitInstruction (instr);
    int idxLine = arrIndexes[0];
    int idsSize = arrIndexes.size ();
    for (int i = 1; i < idsSize; i++)
    {
        int line1 = ++sp;
        Instruction* constInstr = new Instruction (op_const, currentFunc->arrTable [toLoad].second [i], -1, line1);
        int constLine = emitInstruction (constInstr);
        
        int mulLine = ++sp;
        Instruction* mulInstr = new Instruction (op_mul, idxLine, constLine, mulLine);
        int mulres = emitInstruction (mulInstr);
        
        int addLine = ++sp;
        Instruction* addInstr = new Instruction (op_add, mulres, arrIndexes[i], addLine);
        int addres = emitInstruction (addInstr);
        
        idxLine = addres;
    }
    
    line = ++sp;
    Instruction* instr2 = new Instruction (op_mul, idxLine, o1, line);
    o1 = emitInstruction (instr2);
    
    line = ++sp;
    Instruction* dimInstr = new Instruction (op_const, currentFunc->arrTable [toLoad].first, -1, line);
    int dimLine = emitInstruction (dimInstr);
    
    line = ++sp;
    Instruction* instr3 = new Instruction (op_add, -28, dimLine, line);
    o2 = emitInstruction (instr3);
    
    line = ++sp;
    Instruction* addaInstr = new Instruction (op_adda, o1, o2, line);
    int loadArg = line;
    
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
    else result = CSELine;
    return result;
}

int Parser::emitStore (int what, int where)
{
    int result = 0;
    if (arrIndexes.size () != currentFunc->arrTable [where].second.size ()) err ("Incorrect number of indices for the array!");
    int o1 = 0, o2 = 0;
    int line = ++sp;
    Instruction* instr = new Instruction (op_const, SIZEOFINT, -1, line);
    o1 = emitInstruction (instr);
    int idxLine = arrIndexes[0];
    int idsSize = arrIndexes.size ();
    for (int i = 1; i < idsSize; i++)
    {
        int line1 = ++sp;
        Instruction* constInstr = new Instruction (op_const, currentFunc->arrTable [where].second [i], -1, line1);
        int constLine = emitInstruction (constInstr);
        
        int mulLine = ++sp;
        Instruction* mulInstr = new Instruction (op_mul, idxLine, constLine, mulLine);
        int mulres = emitInstruction (mulInstr);
        
        int addLine = ++sp;
        Instruction* addInstr = new Instruction (op_add, mulres, arrIndexes[i], addLine);
        int addres = emitInstruction (addInstr);
        
        idxLine = addres;
    }
    
    line = ++sp;
    Instruction* instr2 = new Instruction (op_mul, idxLine, o1, line);
    o1 = emitInstruction (instr2);
    
    line = ++sp;
    Instruction* dimInstr = new Instruction (op_const, currentFunc->arrTable [where].first, -1, line);
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
