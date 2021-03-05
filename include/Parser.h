#include "Lexer.h"
#include "Module.h"
#include <array>
#include <set>
#include <cstdlib>

#define SIZEOFINT 4
class Parser
{
public:
	Parser (std::string s);
	void parse ();
    void dotGraph (std::string compiledIR);
    
private:
    int bbCounter;
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
	std::vector <int> formalParam ();
	void funcDecl ();
	void varDecl ();
	void typeDecl ();

	void statement ();

	void assignment ();
	int  funcCall (bool isVoid);
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
*	"Stack Pointer" -- depth of a current expression stack (basically, a number of instructions)
*/
	int sp;
    
    Module*     m;
    Function*   currentFunc;
    BasicBlock* currentBB;
    
    int emitInstruction (Instruction* instr);
    
    void ifThenDiamond     (BasicBlock* ifBB,     BasicBlock* thenEntryBB, BasicBlock* thenExitBB,  BasicBlock* fiBB, std::map <int, int>* ifVarTable, std::map <int, int>* thenVarTable );
    void ifThenElseDiamond (BasicBlock* ifBB,     BasicBlock* thenEntryBB, BasicBlock* elseEntryBB, BasicBlock* thenExitBB,  BasicBlock* elseExitBB, BasicBlock* fiBB, std::map <int, int>* ifVarTable, std::map <int, int>* thenVarTable, std::map <int, int>* elseVarTable);
    void whileDoDiamond    (BasicBlock* beforeBB, BasicBlock* whileBB,     BasicBlock* doBB,        BasicBlock* jmpBackBB,   BasicBlock* odBB, std::map <int, int>* beforeLoopVarTable, std::map <int, int>* afterLoopVarTable);
    
    std::set <BasicBlock*> visitedBB;
    bool propagatePhi (BasicBlock* startBB, BasicBlock* endBB, int varID, int SSALine);
    bool replaceWithPhi (BasicBlock* bb, int varID, int SSALine);
    
/**
*  funcTable stores function id (first) and pointer to the Function (second)
*/
    std::map <int, Function* > funcTable;
   
/**
*  arrTable stores array id (index of an element), memory offset (first in a pair), and vector of dimensions (second in a pair)
*/
    int offset = 0;
    std::vector <int> dims;
    bool isArray = false;
    
    int arrID1 = -1;
    int arrID2 = -1;
    std::vector<int> arrIndexes;
    int emitLoad (int toLoad);
    int emitStore (int what, int where);
    
/**
 * Data structure for Common Subexpression Elimination
 */
    std::array <Instruction*, NUMOFOPS> CSE;
    bool pushCSE (Instruction* instr);
    int findCommonSubexpression (Instruction* instr);
    
    std::vector <Instruction* > allInstructions;
};
