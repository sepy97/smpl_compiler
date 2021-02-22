#include "Lexer.h"
#include "Module.h"
#include <array>

class Parser
{
public:
	Parser (std::string s);
	void parse ();
    
    void dotGraph ();
private:
    
    int bbCounter;// = 0;
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
	bool typeDecl ();

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
    void whileDoDiamond    (BasicBlock* beforeBB, BasicBlock* whileBB,     BasicBlock* doBB,        BasicBlock* jmpBackBB,   BasicBlock* odBB, std::map <int, int>* beforeLoopVarTable, std::map <int, int>* afterLoopVarTable);
    
    void propagatePhi (BasicBlock* startBB, BasicBlock* endBB, int varID, int SSALine);
    
    void replaceWithPhi (BasicBlock* bb, int varID, int SSALine);
    
/**
*  varTable stores variable id (first) and SSA line number (second)
*/
    std::map <int, int> varTable;
    
/**
 * Data structure for Common Subexpression Elimination
 */
    std::array <Instruction*, NUMOFOPS> CSE;// [NUMOFOPS];
    bool pushCSE (Instruction* instr);
    Instruction* popCSE (opCode opc);
    int findCommonSubexpression (Instruction* instr);
    
};
