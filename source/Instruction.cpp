#include "Instruction.h"
#include <iostream>

void Instruction::setOp (opCode op)
{
    this->op = op;
}

void Instruction::setOperand1 (Operand operand1)
{
    this->operand1 = operand1;
}

void Instruction::setOperand2 (Operand operand2)
{
    this->operand2 = operand2;
}

std::string Instruction::toString ()
{
    //std::cout << this->op << std::endl;
    
    std::string res = "";
    res += std::to_string (this->SSALine);
    res += ": ";
    switch (this->op)
    {
        case nop:
        {
            res += "@@@@";
            break;
        }
        case op_neg:
        {
            res += "neg ";
            res += operand1.toString ();
            break;
        }
        case op_add:
        {
            res += "add ";
            res += operand1.toString ();
            res += " ";
            res += operand2.toString ();
            break;
        }
        case op_sub:
        {
            res += "sub ";
            res += operand1.toString ();
            res += " ";
            res += operand2.toString ();
            break;
        }
        case op_mul:
        {
            res += "mul ";
            res += operand1.toString ();
            res += " ";
            res += operand2.toString ();
            break;
        }
        case op_div:
        {
            res += "div ";
            res += operand1.toString ();
            res += " ";
            res += operand2.toString ();
            break;
        }
        case op_cmp:
        {
            res += "cmp ";
            res += operand1.toString ();
            res += " ";
            res += operand2.toString ();
            break;
        }
        case op_adda:
        {
            res += "adda ";
            res += operand1.toString ();
            res += " ";
            res += operand2.toString ();
            break;
        }
        case op_load:   //@@@@
        {
            res += "load ";
            res += operand1.toString ();
            break;
        }
        case op_store:  //@@@@
        {
            res += "store ";
            res += operand1.toString ();
            res += " ";
            res += operand2.toString ();
            break;
        }
        case op_phi:
        {
            res += "phi ";
            res += operand1.toString ();
            res += " ";
            res += operand2.toString ();
            break;
        }
        case op_end:
        {
            res += "end ";
            break;
        }
        case op_bra:
        {
            res += "bra ";
            res += operand1.toString ();
            break;
        }
        case op_bne:
        {
            res += "bne ";
            res += operand1.toString ();
            res += " ";
            res += operand2.toString ();
            break;
        }
        case op_beq:
        {
            res += "beq ";
            res += operand1.toString ();
            res += " ";
            res += operand2.toString ();
            break;
        }
        case op_ble:
        {
            res += "ble ";
            res += operand1.toString ();
            res += " ";
            res += operand2.toString ();
            break;
        }
        case op_blt:
        {
            res += "blt ";
            res += operand1.toString ();
            res += " ";
            res += operand2.toString ();
            break;
        }
        case op_bge:
        {
            res += "bge ";
            res += operand1.toString ();
            res += " ";
            res += operand2.toString ();
            break;
        }
        case op_bgt:
        {
            res += "bgt ";
            res += operand1.toString ();
            res += " ";
            res += operand2.toString ();
            break;
        }
        case op_read:
        {
            res += "read ";
            break;
        }
        case op_write:
        {
            res += "write ";
            res += operand1.toString ();
            break;
        }
        case op_writeNL:
        {
            res += "writeNL ";
            break;
        }
        default:
        {
            break;
        }
            
    }
    //std::cout << res << std::endl;
    return res;
}
