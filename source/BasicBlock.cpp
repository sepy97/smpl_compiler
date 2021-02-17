#include "BasicBlock.h"
#include <iostream>

void BasicBlock::pushInstruction (Instruction* toInsert)
{
    this->body.push_back (toInsert);
}

bool BasicBlock::pushAfter (Instruction* toInsert, Instruction* after)
{
    auto it = std::find ( this->body.begin (), this->body.end (), after);
    if (it == std::end (this->body)) return false;
    this->body.insert (it, toInsert);
    return true;
}

void BasicBlock::addPred (BasicBlock* pred)
{
    this->predecessors.push_back (pred);
}

void BasicBlock::addSucc (BasicBlock* succ)
{
    this->successors.push_back (succ);
}

std::string BasicBlock::toString ()
{
    std::string res = "";
    if (this->body.empty () && this->phiInstructions.empty () ) res += "<empty>\n";
    else
    {
        
        for (std::pair<Instruction* , int> i: this->phiInstructions)
        {
                res += i.first->toString ();
                res += "\n";
        }
        
        for (Instruction* i: this->body)
        {
            res += i->toString ();
            res += "\n";
        }
    }
    //std::cout << res << std::endl;
    return res;
}
