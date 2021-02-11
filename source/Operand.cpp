#include "Operand.h"


std::string Operand::toString ()
{
    std::string res = "";
    switch (this->kind)
    {
        case constant:
        {
            res += "#";
            res += std::to_string (val);
            break;
        }
        case SSALine:
        {
            res += "(";
            res += std::to_string (val);
            res += ")";
            break;
        }
        case variable:
        {
            res += "variable ";
            res += std::to_string (val);
            break;
        }
        default:
        {
            break;
        }
    }
    return res;
}
