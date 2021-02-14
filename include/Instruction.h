#include <vector>
#include <string>
//#include "Operand.h"

enum opCode
{
    nop,                //empty opcode
    
    op_neg,
    op_add,
    op_sub,
    op_mul,
    op_div,
    op_cmp,
    
    op_adda,            //add two adresses of arrays
    op_load,
    op_store,
    
    op_phi,
    op_end,
    
    op_bra,
    op_bne,
    op_beq,
    op_ble,
    op_blt,
    op_bge,
    op_bgt,
    
    op_read,
    op_write,
    op_writeNL,
    
    op_const,
};

class Instruction
{
public:
    Instruction (int line)
    {
        this->op = nop;
        this->operand1 = -1;//Operand ();
        this->operand2 = -1;//Operand ();
        this->SSALine = line;
    }
    
    Instruction (opCode op, int operand1, int operand2, int line)
    {
        this->op = op;
        this->operand1 = operand1;
        this->operand2 = operand2;
        this->SSALine = line;
    }
    
    void setOp (opCode op);
    void setOperand1 (int operand1);
    void setOperand2 (int operand2);
    
    bool operator==(const Instruction &i2) const
    {
        return (this->SSALine == i2.SSALine);
    }/*
    bool operator== (const Instruction &i1, const Instruction &i2)
    {
        return (i1.SSALine == i2.SSALine);
    }*/
    
    std::string toString ();
    
private:
    opCode op;
    int operand1, operand2;
    int SSALine;
    
    
};
