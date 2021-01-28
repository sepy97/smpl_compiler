#include "Lexer.h"

enum resultType
{
	constant,
	variable,
	registr,
};

class Parser
{
public:
	Parser (std::string s);
	void parse ();

private:
	class Result
	{

	/**
	*	Result is a value that will be returned by Compute function @@@@
	*/

	public:
		Result ()
		{
			kind = constant;
			val = 0, addr = -1, reg = -1;
		}
		Result (resultType t, int v)
		{
			kind = t;
			switch (t)
			{
				case constant:
				{
					val = v;
					break;
				}
				case variable:
				{
					addr = v;
					break;
				}
				case registr:
				{
					reg = v;
					break;
				}
				default:
				{
					std::cout << "Incorrect resultType (Result constructor): " << t << " " << v << std::endl;
				}
			}
		}

		std::pair <resultType, int> getResult ()
		{
			std::pair <resultType, int> ret;
			ret.first = kind;
			switch (kind)
			{
				case constant:
                                {
                                        ret.second = val;
                                        break;
                                }
                                case variable:
                                {
                                        ret.second = addr;
                                        break;
                                }
                                case registr:
                                {
                                        ret.second = reg;
                                        break;
                                }
                                default:
                                {
                                        std::cout << "Incorrect resultType (function getResult): " << kind << " " << val << std::endl;
                                }
			}
			return ret;
		}
		
	private:
		resultType kind;	/** constant, variable or register */
		int val;	 	/** use this variable if we've computed a constant value */
		int addr;		/** use this variable if we've computed a variable (addr is variable address in varTable */
		int reg;		/** use this variable if we've computed a registr (reg is a number of a registr) */
	};

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
	void funcCall ();
	void ifStatement ();
	void whileStatement ();
	void returnStatement ();

	void designator ();
	void factor ();
	void term ();
	void expression ();
	void relation ();

	void ident ();
	void number ();

	void relOp ();
	//@@@@ void digit ();
	//@@@@ void letter ();

	
/**
*	"Stack Pointer" -- depth of a current expression stack
*/
	int sp;

};
