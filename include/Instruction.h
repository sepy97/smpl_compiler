#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#define NUMOFOPS 50     /** Fixed number of possible opcodes */

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
    
    op_call,
    op_mu,
    op_ret,
    
    op_const,
};

class Instruction
{
public:
    Instruction (int line)
    {
        this->op = nop;
        this->operand1 = -1;
        this->operand2 = -1;
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
    
    void setVar1 (int varID);
    void setVar2 (int varID);
    
    void setArr1 (int ID);
    void setArr2 (int ID);
    
    bool operator==(const Instruction &i2) const
    {
        return (this->SSALine == i2.SSALine);
    }
    
    bool compare (const Instruction* i1)
    {
        return (i1->op == this->op && i1->operand1 == this->operand1 && i1->operand2 == this->operand2);
    }
    
    int getLine ()
    {
        return SSALine;
    }
    
    opCode getOp ()
    {
        return this->op;
    }
    
    int getOperand1 ()
    {
        return this->operand1;
    }
    
    int getOperand2 ()
    {
        return this->operand2;
    }
    
    int getVar1 ()
    {
        return this->var1;
    }
    
    int getVar2 ()
    {
        return this->var2;
    }
    
    int getArr1 ()
    {
        return this->arr1;
    }
    
    int getArr2 ()
    {
        return this->arr2;
    }
    
    std::string toString ();
    
    
    void setPrevDom (Instruction* instr)
    {
        this->prevDomInstr = instr;
    }
    
    Instruction* getPrevDom ()
    {
        return this->prevDomInstr;
    }
    
    int addaArg1 = -1;
    int addaArg2 = -1;
    
    std::vector <int> funcArgs;
    
    std::vector <int> referredVariables; // IDs of all variables to which this instruction was assigned
    
private:
    opCode op;
    int operand1, operand2;
    int SSALine;
    
    int var1 = -1, var2 = -1;
    
    int arr1 = -1, arr2 = -1;
    
    Instruction* prevDomInstr = nullptr;
    
    
};
