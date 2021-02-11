#include <string>

enum operandType
{
    constant,
    SSALine,
    variable,   
    type_read,
    type_write,
};

class Operand
{
public:
    Operand ()
    {
        this->kind    = constant;
        this->val     = 0;
    }
    Operand (int v)     /** Create operand with a constant value */
    {
        this->kind = constant;
        this->val = v;
    }
    Operand (operandType k, int v)
    {
        this->kind    = k;
        this->val     = v;
    }
    
    int getKind ()
    {
        return this->kind;
    }
    
    int getVal ()
    {
        return this->val;
    }
    
    std::string toString ();
    
private:
    //int lineNum;
    operandType kind;
    int val;
    
};
