#include "Instruction.h"
#include <iostream>

void Instruction::setOp (opCode op)
{
    this->op = op;
}

void Instruction::setOperand1 (int operand1)
{
    this->operand1 = operand1;
}

void Instruction::setOperand2 (int operand2)
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
            res += std::to_string (operand1);
            break;
        }
        case op_add:
        {
            res += "add ";
            res += std::to_string (operand1);
            res += " ";
            res += std::to_string (operand2);
            break;
        }
        case op_sub:
        {
            res += "sub ";
            res += std::to_string (operand1);
            res += " ";
            res += std::to_string (operand2);
            break;
        }
        case op_mul:
        {
            res += "mul ";
            res += std::to_string (operand1);
            res += " ";
            res += std::to_string (operand2);
            break;
        }
        case op_div:
        {
            res += "div ";
            res += std::to_string (operand1);
            res += " ";
            res += std::to_string (operand2);
            break;
        }
        case op_cmp:
        {
            res += "cmp ";
            res += std::to_string (operand1);
            res += " ";
            res += std::to_string (operand2);
            break;
        }
        case op_adda:
        {
            res += "adda ";
            res += std::to_string (operand1);
            res += " ";
            res += std::to_string (operand2);
            break;
        }
        case op_load:   //@@@@
        {
            res += "load ";
            res += std::to_string (operand1);
            break;
        }
        case op_store:  //@@@@
        {
            res += "store ";
            res += std::to_string (operand1);
            res += " ";
            res += std::to_string (operand2);
            break;
        }
        case op_phi:
        {
            res += "phi ";
            res += std::to_string (operand1);
            res += " ";
            res += std::to_string (operand2);
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
            res += std::to_string (operand1);
            break;
        }
        case op_bne:
        {
            res += "bne ";
            res += std::to_string (operand1);
            res += " ";
            res += std::to_string (operand2);
            break;
        }
        case op_beq:
        {
            res += "beq ";
            res += std::to_string (operand1);
            res += " ";
            res += std::to_string (operand2);
            break;
        }
        case op_ble:
        {
            res += "ble ";
            res += std::to_string (operand1);
            res += " ";
            res += std::to_string (operand2);
            break;
        }
        case op_blt:
        {
            res += "blt ";
            res += std::to_string (operand1);
            res += " ";
            res += std::to_string (operand2);
            break;
        }
        case op_bge:
        {
            res += "bge ";
            res += std::to_string (operand1);
            res += " ";
            res += std::to_string (operand2);
            break;
        }
        case op_bgt:
        {
            res += "bgt ";
            res += std::to_string (operand1);
            res += " ";
            res += std::to_string (operand2);
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
            res += std::to_string (operand1);
            break;
        }
        case op_writeNL:
        {
            res += "writeNL ";
            break;
        }
        case op_const:
        {
            res += std::to_string (operand1);
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
