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

void Instruction::setVar1 (int varID)
{
    this->var1 = varID;
}

void Instruction::setVar2 (int varID)
{
    this->var2 = varID;
}

void Instruction::setArr1 (int ID)
{
    this->arr1 = ID;
}

void Instruction::setArr2 (int ID)
{
    this->arr2 = ID;
}

std::string Instruction::toString ()
{
    std::string res = "";
    res += std::to_string (this->SSALine);
    res += ": ";
    switch (this->op)
    {
        case nop:
        {
            res += "NOP";
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
            if (operand1 == -28)
            {
                res += "add R28 (";
                res += std::to_string (operand2);
                res += ")";
            }
            else
            {
                res += "add (";
                res += std::to_string (operand1);
                res += ") (";
                res += std::to_string (operand2);
                res += ")";
            }
            break;
        }
        case op_sub:
        {
            res += "sub (";
            res += std::to_string (operand1);
            res += ") (";
            res += std::to_string (operand2);
            res += ")";
            break;
        }
        case op_mul:
        {
            res += "mul (";
            res += std::to_string (operand1);
            res += ") (";
            res += std::to_string (operand2);
            res += ")";
            break;
        }
        case op_div:
        {
            res += "div (";
            res += std::to_string (operand1);
            res += ") (";
            res += std::to_string (operand2);
            res += ")";
            break;
        }
        case op_cmp:
        {
            res += "cmp (";
            res += std::to_string (operand1);
            res += ") (";
            res += std::to_string (operand2);
            res += ")";
            break;
        }
        case op_adda:
        {
            res += "adda (";
            res += std::to_string (operand1);
            res += ") (";
            res += std::to_string (operand2);
            res += ")";
            break;
        }
        case op_load:
        {
            res += "load (";
            res += std::to_string (operand1);
            res += ")";
            break;
        }
        case op_store:
        {
            res += "store (";
            res += std::to_string (operand1);
            res += ") (";
            res += std::to_string (operand2);
            res += ")";
            break;
        }
        case op_phi:
        {
            res += "phi (";
            res += std::to_string (operand1);
            res += ") (";
            res += std::to_string (operand2);
            res += ")";
            break;
        }
        case op_end:
        {
            res += "end ";
            break;
        }
        case op_bra:
        {
            res += "bra (";
            res += std::to_string (operand1);
            res += ")";
            break;
        }
        case op_bne:
        {
            res += "bne (";
            res += std::to_string (operand1);
            res += ") (";
            res += std::to_string (operand2);
            res += ")";
            break;
        }
        case op_beq:
        {
            res += "beq (";
            res += std::to_string (operand1);
            res += ") (";
            res += std::to_string (operand2);
            res += ")";
            break;
        }
        case op_ble:
        {
            res += "ble (";
            res += std::to_string (operand1);
            res += ") (";
            res += std::to_string (operand2);
            res += ")";
            break;
        }
        case op_blt:
        {
            res += "blt (";
            res += std::to_string (operand1);
            res += ") (";
            res += std::to_string (operand2);
            res += ")";
            break;
        }
        case op_bge:
        {
            res += "bge (";
            res += std::to_string (operand1);
            res += ") (";
            res += std::to_string (operand2);
            res += ")";
            break;
        }
        case op_bgt:
        {
            res += "bgt (";
            res += std::to_string (operand1);
            res += ") (";
            res += std::to_string (operand2);
            res += ")";
            break;
        }
        case op_read:
        {
            res += "read ";
            break;
        }
        case op_write:
        {
            res += "write (";
            res += std::to_string (operand1);
            res += ")";
            break;
        }
        case op_writeNL:
        {
            res += "writeNL ";
            break;
        }
        case op_call:
        {
            res += "call [BB";
            res += std::to_string (operand1);
            res += "]";
            for (int i = 0; i < funcArgs.size (); i++)
            {
                res += "(";
                res += std::to_string (funcArgs [i]);
                res += ")";
            }
            break;
        }
        case op_mu:
        {
            res += "Parameter ";
            res += std::to_string (operand1);
            break;
        }
        case op_const:
        {
            res += "const #";
            res += std::to_string (operand1);
            break;
        }
        case op_ret:
        {
            res += "ret (";
            res += std::to_string (operand1);
            res += ")";
            break;
        }
        default:
        {
            std::cout << "toString: Incorrect instruction opcode!" << std::endl;
            break;
        }
    }
    return res;
}


