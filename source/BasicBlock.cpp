#include "BasicBlock.h"
#include <iostream>

void BasicBlock::pushInstruction (Instruction* toInsert)
{
    if (!this->body.empty ())
    {
        Instruction* lastInstr = this->body.back ();
        /*if (lastInstr->getOp () == nop)
        {
            lastInstr->setOp (toInsert->getOp ());
            lastInstr->setOperand1 (toInsert->getOperand1 ());
            lastInstr->setOperand2 (toInsert->getOperand2 ());
            lastInstr->setVar1 (toInsert->getVar1 ());
            lastInstr->setVar2 (toInsert->getVar2 ());
        }
        else*/
        this->body.push_back (toInsert);
    }
    else this->body.push_back (toInsert);
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

void BasicBlock::dotGraph (std::string* basicBlocks, std::string* edges)
{
    if (this->body.empty () && this->phiInstructions.empty () )
    {
        //res += "<empty>\n";
        *basicBlocks += " \\< empty \\> ";
    }
    else
    {
        
        for (std::pair<Instruction* , int> i: this->phiInstructions)
        {
            *basicBlocks += i.first->toString ();
            *basicBlocks += " | ";
            //dotGraph (basicBlocks, edges);
               // res += i.first->toString ();
               // res += "\n";
        }
        
        for (Instruction* i: this->body)
        {
            *basicBlocks += i->toString ();
            *basicBlocks += " | ";
            //if (i != this->body.end ()) *basicBlocks += " | ";
            //i->dotGraph (basicBlocks, edges);
            //res += i->toString ();
            //res += "\n";
        }
        (*basicBlocks).pop_back ();
        (*basicBlocks).pop_back ();
    }
}
