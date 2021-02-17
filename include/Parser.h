#include "Lexer.h"
#include "Module.h"

class Parser
{
public:
	Parser (std::string s);
	void parse ();

private:
    
/**
*	"Lexer" will get tokens from a source file (more info in Lexer.h)
*/
	Lexer lex;
	void err (std::string arg);

/**
*	Functions required by EBNF for smpl
*/
	void statSequence ();
	void computation ();
	void funcBody ();
	void formalParam ();
	void funcDecl ();
	void varDecl ();
	void typeDecl ();

	void statement ();

	void assignment ();
	int  funcCall ();
	void ifStatement ();
	void whileStatement ();
	void returnStatement ();

	int  designator ();
	int  factor ();
	int  term ();
	int  expression ();
	void relation ();
	void relOp ();

/**
*	"Stack Pointer" -- depth of a current expression stack
*/
	int sp;
    
    Module*     m;
    Function*   currentFunc;
    BasicBlock* currentBB;
    
    BasicBlock* currentJoinBB; //@@@@
    
    void ifThenDiamond     (BasicBlock* ifBB,     BasicBlock* thenEntryBB, BasicBlock* thenExitBB,  BasicBlock* fiBB, std::map <int, int>* ifVarTable, std::map <int, int>* thenVarTable );
    void ifThenElseDiamond (BasicBlock* ifBB,     BasicBlock* thenEntryBB, BasicBlock* elseEntryBB, BasicBlock* thenExitBB,  BasicBlock* elseExitBB, BasicBlock* fiBB, std::map <int, int>* ifVarTable, std::map <int, int>* thenVarTable, std::map <int, int>* elseVarTable);
    void whileDoDiamond    (BasicBlock* beforeBB, BasicBlock* whileBB,     BasicBlock* doBB,        BasicBlock* jmpBackBB,   BasicBlock* odBB);
    
/**
*  varTable stores variable id (first) and SSA line number (second)
*/
    std::map <int, int> varTable;
};
