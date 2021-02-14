#include "Lexer.h"
#include "Module.h"
/*
enum resultType
{
	constant,
	//variable,
    instruction,
};*/
/*
enum resultOp
{
    resultAdd,
    resultSub,
    resultMul,
    resultDiv,
};
  */

class Parser
{
public:
	Parser (std::string s);
	void parse ();

private:
/*	class Result
	{


	public:
		Result ()
		{
			kind = constant;
            val = 0;
            //, addr = -1, reg = -1;
		}
        Result (resultType kind, int arg)
        {
            this->kind = kind;
            switch (kind)
            {
                case constant:
                {
                    this->val = arg;
                    break;
                }
                case variable:
                {
                    this->addr = arg;
                    break;
                }
                case instruction:
                {
                    this->ssaline = arg;
                    break;
                }
                default:
                {
                    std::cout << "Incorrect resultType (Result constructor): " << kind << " " << arg << std::endl;
                }
            }
            //this->val = v;
        }
        

        resultType getKind ()
        {
            return kind;
        }
        
        int getVal ()
        {
            return val;
        }
        
        
		
	private:
		resultType kind;
		int val;
        //int addr;
        //int ssaline;
        //Instruction* inst;
	};*/

/**
*	"Lexer" will get tokens from a source file (more info in Lexer.h)
*/
	Lexer lex;
	void err (std::string arg);

	//@@@ varTable;

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
	int funcCall ();
	void ifStatement ();
	void whileStatement ();
	void returnStatement ();

	int designator ();
	int factor ();
	int term ();
	int expression ();
	void relation ();

	//int ident ();
	//Operand number ();

	void relOp ();
	//@@@@ void digit ();
	//@@@@ void letter ();
    
/**
*    "Stack Pointer" -- depth of a current expression stack
*/
    //int buf [];
	
/**
*	"Stack Pointer" -- depth of a current expression stack
*/
	int sp;
    
    Module* m;
    Function* currentFunc;
    BasicBlock* currentBB;
    
    
/**
 *  varTable stores variable id (first) and SSA line number (second)
 */
    std::map <int, int> varTable;
};
