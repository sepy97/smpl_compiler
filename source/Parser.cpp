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
        
        std::map <int, int> params; // @@@@
        
        Function* f = new Function (0 /* main funcID */, params, bb);
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
    int isVoid = false;
    if (tk == tk_void)
    {
        lex.next ();                /** Consuming tk_void */
        isVoid = true;
        tk = lex.getToken ();
    }
    if (tk != tk_function) err ("Incorrect function declaration (missing 'function' keyword)!");
    lex.next ();                    /** Consuming tk_function */
    
    //ident ();
    tk = lex.getToken ();
    if (tk != identifier) err ("Incorrect function declaration!");
    int funcID = lex.getId ();
    
    //funcTable.insert (funcID);
    
    lex.next ();                    /** Consuming function identifier */
    
    std::vector <int> paramList = formalParam ();
    
    tk = lex.getToken ();
    if (tk != semicolon) err ("Syntax error in function declaration: missing semicolon after function header!");
    lex.next ();                    /** Consuming semicolon */
    
    //create function with isVoid, funcID and params
    //make it a current func, save previous func
    Function* prevFunc = currentFunc;
    BasicBlock* prevBB = currentBB;
    BasicBlock* bb = new BasicBlock (++bbCounter);
    currentBB = bb;
    std::map <int, int> params;
    
  //  std::cout << "paramList.size = " << paramList.size () << std::endl;
    
    for (int i = 0; i < paramList.size (); i++)
    {
        
    //    std::cout << "paramList: " << paramList [i] << std::endl;
        params [paramList [i]] = -5;// currentFunc->varTable [paramList [i]];
    }
    Function* f = new Function (funcID, params, currentBB);
    
    funcTable [funcID] = f;
    
    m->insertFunc (f);
    currentFunc = f;
    
  //  std::cout << "funcDecl: " << lex.getToken () << std::endl;
    
    funcBody ();
    
    //restore current func
    currentFunc = prevFunc;
    currentBB = prevBB;
    
    //restore currentBB @@@@???
    
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
    
    for (auto& x : currentFunc->params)
    {
        //int line = ++sp;
        Instruction* instr = new Instruction (op_mu, x.first, -1, ++sp);
        int ssaLine = emitInstruction (instr);
        currentFunc->varTable [x.first] = ssaLine;
    }
    
    tk = lex.getToken ();
  //  std::cout << "should be curlBracket token: " << tk << std::endl;
    if (tk != openCurlBracket) err ("Syntax error: missing opening curling bracket!");
    lex.next ();                    /** Consuming curl bracket */
    tk = lex.getToken ();
//    std::cout << "before statSequence token: " << tk << std::endl;
    if (tk == tk_let || tk == tk_call || tk == tk_if || tk == tk_while || tk == tk_return)
    {
        statSequence ();
        
    }
    
    // lex.next (); //@@@@
    tk = lex.getToken ();
   // std::cout << "funcBody token: " << tk << std::endl;
    
    if (tk != closeCurlBracket) err ("Syntax error: missing closing curling bracket!");
    lex.next ();                    /** Consuming curl bracket */
}

void Parser::statSequence ()
{
    token tk = lex.getToken ();
  //  std::cout << "statSequence token: " << tk << std::endl;
    
    while (tk == tk_let || tk == tk_call || tk == tk_if || tk == tk_while || tk == tk_return)
    {
        statement ();
        
        tk = lex.getToken ();
        
    // std::cout << "AFTER STATEMENT statSequence token: " << tk << std::endl;
        if (tk == semicolon)
        {
            lex.next ();            /** Consuming semicolon */
            tk = lex.getToken ();
  //          std::cout << "SEMICOLON statSequence token: " << tk << std::endl;
        }
        
//        std::cout << "END OF THE WHILE statSequence token: " << tk << std::endl;
    }
    
    //std::cout <<
}

void Parser::statement ()
{
    token tk = lex.getToken ();
    
  //  std::cout << "statement token: " << tk << std::endl;
    
	switch (tk)
	{
		case tk_let:
		{
			assignment ();
            
            
  //          std::cout << "RIGHT AFTER ASSIGNMENT "<< std::endl;

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
     //       std::cout << "assigning a SSALine to a variable before phiCheck: " << o2 << std::endl;
            
            //@@@@  insert varID to referredVariables  phiCheck
            int varID = o1;
            int searchingLine = o2;
            Instruction* instr;
            
    //        std::cout << "before loop! " << std::endl;
            
            for (int j = allInstructions.size (); --j >= 0;)
            {
                if (allInstructions [j]->getLine () == searchingLine)
                {
                    instr = allInstructions [j];
                }
            }
            
  //          std::cout << "after loop! " << std::endl;
            //@@@@ get an assigned instruction
            
            if (std::find (instr->referredVariables.begin (), instr->referredVariables.end (), varID) == instr->referredVariables.end ())
            {
                instr->referredVariables.push_back (varID);
            }
            //@@@@
     //       std::cout << "after FIND! " << std::endl;
            
            currentFunc->varTable [o1] = o2;
        }
        else if (o2 < 0)
        {
    //        std::cout << "assigning a variable to a variable before phiCheck: " << o2 << std::endl;
            //@@@@  insert varID to referredVariables  phiCheck
            int varID = o1;
            int searchingLine = currentFunc->varTable [-o2];
            Instruction* instr;
            for (int j = allInstructions.size (); --j >= 0;)
            {
                if (allInstructions [j]->getLine () == searchingLine)
                {
                    instr = allInstructions [j];
                }
            }
            
            //@@@@ get an assigned instruction
            
            if (std::find (instr->referredVariables.begin (), instr->referredVariables.end (), varID) == instr->referredVariables.end ())
            {
                instr->referredVariables.push_back (varID);
            }
            //@@@@
            
            currentFunc->varTable [o1] = currentFunc->varTable [-o2];
        }
        else err ("Assigning non-existing SSA line");
    }
    
//    std::cout << "end of assignment " << std::endl;
}

int Parser::funcCall ()
{
    int result = 0;
    
    lex.next ();                    /** Consuming call */
    token tk = lex.getToken ();
    if (tk == tk_read)
    {
        //std::cout << "InputNum: " << tk << std::endl;
        
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
        lex.next ();                /** Consuming tk_write */
        lex.next ();                /** Consuming bracket */
        
        tk = lex.getToken ();
        int o1 = expression ();
        int line = 0;//++sp;
        
        int operand1 = o1;
        int var1 = -1;
        
        if (isArray)
        {
            line = ++sp;
            
            arrID1 = o1;
            operand1 = emitLoad (o1);//line;
            
        }
        else
        {
            if (o1 < 0)
            {
                var1 = -o1;
                operand1 = currentFunc->varTable [var1];
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
    //    std::cout << "INSIDE of funcCall" << std::endl;
        
        int line = 0;
        std::vector <int> funcArgs;
        int funcID = lex.getId ();
        if (funcTable.find (funcID) == funcTable.end()) err ("Call of undeclared function!");
        
        lex.next ();
        token tk = lex.getToken ();
        if (tk == openBracket)
        {
            lex.next ();                /** Consuming bracket */
            
   //             std::cout << "AFTER consuming an opening bracket" << std::endl;
            
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
                
                //lex.next ();
            }
            
   //             std::cout << "BEFORE consuming a closing bracket" << std::endl;
            
            tk = lex.getToken ();
            if (tk != closeBracket) err ("Syntax error: missing closing bracket!");
            lex.next ();                /** Consuming bracket */
        }
        
        Function* f = funcTable [funcID];
        if (f->params.size () != funcArgs.size ()) err ("Incorrect number of arguments in a function call!");
        
//        std::cout << f->params.size () << " " << funcArgs.size () << std::endl;
        
        /*for (int i = 0; i < f->params.size (); i++)
        {
            std::cout << "params: " << f->params [i] << std::endl;
        }*/
  //          std::cout << "NUMBER of params checked" << std::endl;
        
        line = ++sp;
        Instruction* instr = new Instruction (op_call, f->getEntry ()->label, -1, line);
        //instr->funcArgs = std::vector <int> (funcArgs);
        int paramIncrement = 0;
        for (auto& x : f->params)
        {
       /*     std::cout << x.first  // string (key)
                      << ':'
                      << x.second // string's value
                      << std::endl;*/
            x.second = funcArgs [paramIncrement];
            f->varTable [x.first] = funcArgs [paramIncrement];
            instr->funcArgs.push_back (funcArgs [paramIncrement]);
            paramIncrement++;
        }
        /*for (int i = 0; i < funcArgs.size (); i++)
        {
            std::cout << "funcArgs: " << i << std::endl;
            instr->funcArgs [i] = funcArgs [i];
            
            f->varTable [f->params [i]] = funcArgs [i];
        }*/
        result = emitInstruction (instr);
    }
    
    return result;
    
 //       std::cout << "FINISHED func call" << std::endl;
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
	
    std::map <int, int> ifVarTable (currentFunc->varTable);          /** Making a local copy of varTable before all possible assignments in thenBB */
    
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
    
    std::map <int, int> thenVarTable (currentFunc->varTable);          /** Making a local copy of varTable after thenBB and before all possible assignments in elseBB */
    
    currentFunc->varTable = std::map <int, int> (ifVarTable);
    
    CSE = backupCSE;
    
	tk = lex.getToken ();
    bool wasElse = false;
	if (tk == tk_else)
	{
        wasElse = true;
        line = ++sp;
        
        //NEED CSE ??
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
    
    //@@@@ add phi functions
    line = ++sp;
    
    //NEED CSE ??
    Instruction* fiInstr = new Instruction (line);
    fiBB->pushInstruction (fiInstr);
    allInstructions.push_back (fiInstr);
    
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
    std::map <int, int> beforeLoopVarTable (currentFunc->varTable);          /** Making a local copy of varTable before all possible assignments in doBB */
	
    statSequence ();
    
  //  std::cout << "in a while after statSequence " << std::endl;
    
    BasicBlock* jmpBackBB = currentBB;
    int line = ++sp;
    
    //NEED CSE ??
    Instruction* braInstr = new Instruction (op_bra, -1 /* yet unknown */, -1, line);
    jmpBackBB->pushInstruction (braInstr);
    allInstructions.push_back (braInstr);
    //branch doesn't require CSE
    std::map <int, int> afterLoopVarTable (currentFunc->varTable);          /** Making a local copy of varTable after all assignments in jmpBackBB */

	tk = lex.getToken ();
	if (tk != tk_od) err ("Syntax error in a while statement!");
    lex.next ();                    /** Consuming od */
    
    //NEED CSE ??
    Instruction* odInstr = new Instruction (++sp);
    odBB->pushInstruction (odInstr);
    allInstructions.push_back (odInstr);
    
  //  std::cout << "before whileDoDiamond " << std::endl;
    
    whileDoDiamond (beforeBB, whileBB, doBB, jmpBackBB, odBB, &beforeLoopVarTable, &afterLoopVarTable);
    
 //   std::cout << "after whileDoDiamond " << std::endl;
    
    currentBB = odBB;
    
 //   std::cout << "end of a while statement " << std::endl;
}

void Parser::returnStatement ()
{
    lex.next ();                    /** Consuming return */
    token tk = lex.getToken ();
    
 //   std::cout << "Return statement before expression: " << tk << std::endl;
    if (tk == identifier || tk == num || tk == openBracket || tk == tk_call)
    {
        currentFunc->returningValue = expression ();
        
    }
    
    //lex.next ();
 //   std::cout << "Return statement: " << lex.getToken () << std::endl;
    //lex.next ();
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
    Instruction* cmpInstr = new Instruction (opc, cmpLine, -1/*unknown SSALine*/, line);
    currentBB->pushInstruction (cmpInstr);
    allInstructions.push_back (cmpInstr);
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
    
   // std::cout << "term factor: " << o1 << " " << lex.getToken () << std::endl;
    
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
           // std::cout << "Factor token: " << tk << std::endl;
            //std::cout << m->toString () << std::endl;
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
                Instruction* instr = new Instruction (op_phi, (*thenVarTable) [var.first], (*ifVarTable) [var.first], line);
                //@@@@
                instr->setVar1 (var.first);
                instr->setVar2 (var.first);
                //@@@@
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
                Instruction* instr = new Instruction (op_phi, (*thenVarTable) [var.first], (*elseVarTable) [var.first], line);
                //@@@@
                instr->setVar1 (var.first);
                instr->setVar2 (var.first);
                //@@@@
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
    
   // jmpBackBB->successors.push_back (odBB);
   // odBB->predecessors.push_back    (jmpBackBB);
    
    whileBB->successors.push_back   (odBB);
    odBB->predecessors.push_back    (whileBB);
    
    
    /*
     @@@@
     ADD PHI instructions!
     */
    
  //  std::cout << "beforeLoopVarTable " << std::endl;
    
    for (auto const& var: *beforeLoopVarTable)
    {
        if ( (*beforeLoopVarTable) [var.first] != (*afterLoopVarTable) [var.first])
        {
            if ((*beforeLoopVarTable) [var.first] != 0 && (*afterLoopVarTable) [var.first] != 0) /** Variable is initialized on both paths */
            {
                int line = ++sp;
                Instruction* instr = new Instruction (op_phi, (*beforeLoopVarTable) [var.first], (*afterLoopVarTable) [var.first], line);
                //@@@@
                instr->setVar1 (var.first);
                instr->setVar2 (var.first);
                //@@@@
                whileBB->phiInstructions.push_back ( std::pair<Instruction*, int> (instr, var.first));
                
                allInstructions.push_back (instr);   //@@@@
                
                currentFunc->varTable [var.first] = line;
                
                //@@@@ check phi nodes phiCheck
                int varID = var.first;
                //Instruction* instr;
                //@@@@ get an assigned instruction
                
                if (std::find (instr->referredVariables.begin (), instr->referredVariables.end (), varID) == instr->referredVariables.end ())
                {
                    instr->referredVariables.push_back (varID);
                }
                //@@@@
                
                
           //     std::cout << "PROPAGATING PHI!" << std::endl;
                
                //std::cout << beforeBB->toString () << std::endl;
                //std::cout << whileBB->toString () << std::endl;
                
                /*for (auto& t : varTable)
                    std::cout << t.first << " " << t.second << std::endl;
                */
                
                visitedBB.clear ();
                
                bool afterLine = false;
                
                for (auto& p: whileBB->phiInstructions)
                {
                    if (afterLine)
                    {
                        if (p.first->getVar1 () == varID)
                        {
                            p.first->setOperand1 (line);
                        }
                        
                        if (p.first->getVar2 () == varID)
                        {
                            p.first->setOperand2 (line);
                        }
                        
                        if (find (p.first->referredVariables.begin (), p.first->referredVariables.end (), varID) != p.first->referredVariables.end ())
                        {
                            break;
                            /*
                            isFinished = true;
                            return isFinished;*/
                        }
                    }
                    else
                    {
                        if (p.first->getLine () == line) afterLine = true;
                            
                    }
                             
                }
                
                for (auto& i: whileBB->body)
                {
                    if (i->getVar1 () == varID)
                    {
                        i->setOperand1 (line);
                    }
                
                    if (i->getVar2 () == varID)
                    {
                        i->setOperand2 (line);
                    }
                    
                    if (find (i->referredVariables.begin (), i->referredVariables.end (), varID) != i->referredVariables.end ())
                    {
                        break;
                        /*isFinished = true;
                        return isFinished;*/
                    }
                }
                
                //@@@@ replaceWithPhi (whileBB, var.first, line);
                
            //    std::cout << "REPLACING PHI!" << std::endl;
                std::cout << ++debugInfo << std::endl;
                
                propagatePhi (/*whileBB*/doBB, jmpBackBB, var.first, line);
                
          //      std::cout << "DONE PHI!" << std::endl;
                
            }
        }
    }
    
 //   std::cout << "AFTER beforeLoopVarTable " << std::endl;
    
    
    Instruction* jmpBackAddr = jmpBackBB->body.back ();
    int whileLine = whileBB->body.front ()->getLine ();
    if (! whileBB->phiInstructions.empty ()) whileLine = whileBB->phiInstructions.front ().first->getLine ();
    jmpBackAddr->setOperand1 (whileLine);// ->pushInstruction (new Instruction (op_bra, whileLine, -1, line));
    
    Instruction* whileBranch = whileBB->body.back ();
    int odLine = odBB->body.front ()->getLine ();
    if (! odBB->phiInstructions.empty ()) odLine = odBB->phiInstructions.front ().first->getLine ();
    whileBranch->setOperand2 (odLine);
}

bool /*void*/ Parser::propagatePhi (BasicBlock* startBB, BasicBlock* endBB, int varID, int SSALine)
{
    
    //std::vector <BasicBlock* > propagatingQueue;
    BasicBlock* processedBB = startBB;
    bool isFinished = false;
    
    while (true)
    {
        std::cout << "PHI PROPAGATION: \n" << processedBB->toString () << std::endl;
        
        isFinished = replaceWithPhi (processedBB, varID, SSALine);
        if (processedBB->successors.empty ()) return isFinished;
        
        if (processedBB == endBB) return isFinished;
        
        if (isFinished) return true;
        
        if (processedBB->successors.size () > 1)
        {
            for (auto& bb: processedBB->successors)
            {
                //if (isFinished) return true;
                if ((visitedBB.insert (processedBB)).second == true )
                {
//                    isFinished =
                    isFinished = propagatePhi (bb, endBB, varID, SSALine);
                    //if (isFinished)
                }
                
            }
            return isFinished;
        }
        else
        {
            processedBB = processedBB->successors.front ();
        }
    }
    
    //false;
    
    //if startBB
    //std::cout << startBB->toString () << std::endl;
    
    //std::cout << endBB->toString () << std::endl;
  //  std::cout << "@@@@@@@\n" << std::endl;
    
    //std::set <BasicBlock*> visited;
    //BasicBlock* currentBB = startBB;
    //while (true)
    //if (! isFinished)
    //propagatingQueue.push_back (startBB);
    /*
    while (! propagatingQueue.empty ())
    {
        BasicBlock* poppedBB = propagatingQueue.back ();
        
        std::cout << poppedBB->toString () << std::endl;
        
        propagatingQueue.pop_back ();
        if ((visited.insert (poppedBB)).second == true )
        {
            isFinished = replaceWithPhi (poppedBB, varID, SSALine);
        
            if (isFinished) break;
        //    std::cout << poppedBB->toString () << std::endl;
            
//            std::cout << endBB->toString () << std::endl;
            //if (poppedBB == endBB) break;//return isFinished;
            
            if (poppedBB->successors.empty ()) break;
            
            for (auto& bb: poppedBB->successors)
            {
                if (isFinished) break;
                //isFinished = propagatePhi (bb, endBB, varID, SSALine);
                propagatingQueue.push_back (bb);
            }
        }
        
    }*/
    
    //propagatingQueue.clear ();
    return isFinished;
}

bool /*void*/ Parser::replaceWithPhi (BasicBlock* bb, int varID, int SSALine)
{
    bool isFinished = false;
    
    if (bb->body.empty () && bb->phiInstructions.empty ()) return isFinished;
    
    for (auto& p: bb->phiInstructions)
    {
        std::cout << "PHI node: " << p.first->toString () << std::endl;
        
        if (p.first->getVar1 () == varID)
        {
            p.first->setOperand1 (SSALine);
        }
        
       /* if (p.first->getVar2 () == varID)
        {
            p.first->setOperand2 (SSALine);
        }*/
        
        if (find (p.first->referredVariables.begin (), p.first->referredVariables.end (), varID) != p.first->referredVariables.end ())
        {
            isFinished = true;
            return isFinished;
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
        
        if (find (i->referredVariables.begin (), i->referredVariables.end (), varID) != i->referredVariables.end ())
        {
            isFinished = true;
            return isFinished;
        }
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
            allInstructions.push_back (instr);
            pushCSE (instr);
            result = instr->getLine ();
            break;
        }
        case op_adda:
        {
            currentBB->pushInstruction (instr);
            allInstructions.push_back (instr);
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
                allInstructions.push_back (instr);
                pushCSE (instr);
                result = instr->getLine ();
            } else result = CSELine;
            break;
        }
        default:
        {
            //std::cout << "NO CSE for \n" << instr->toString () << std::endl;
            currentBB->pushInstruction (instr);
            allInstructions.push_back (instr);
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
