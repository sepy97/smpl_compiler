#include "Function.h"
#include <queue>
#include <set>
#include <iostream>

void Function::pushBackBB (BasicBlock* toInsert)
{
    this->exit->addSucc (toInsert);
    toInsert->addPred   (this->exit);
    this->exit = toInsert;
}

void Function::pushFrontBB (BasicBlock* toInsert)
{
    toInsert->addSucc    (this->entry);
    this->entry->addPred (toInsert);
    this->entry = toInsert;
}

void Function::pushAfterBB (BasicBlock* toInsert, BasicBlock* after)
{
    //@@@@
}

std::string Function::toString ()
{
    std::string res = "";
    res += this->label;
    int bbCounter = 0;
    std::queue <BasicBlock*> q;
    std::set <BasicBlock*> visited;
    if (this->entry)
    {
        q.push (this->entry);
        while (!q.empty ())
        {
                BasicBlock* BB = q.front ();
            
            //std::cout << BB->body.size () << std::endl;
            
                if ((visited.insert (BB)).second == true )
                {
                    res += "BB";
                    res += std::to_string (bbCounter);
                    bbCounter++;
                    res += ":\n";
                    res += BB->toString ();
                    res += "########\n";
                    for (BasicBlock* succ: BB->successors)
                         //succ_iterator SI = succ_begin (BB); SI != succ_end (BB); ++SI)
                    {
                            q.push (succ);
                    }
                    
                    //std::cout << res << std::endl;
                }
                q.pop ();
        }
    }
    else res += "{empty}\n";
    res += "-------------------------------------\n";
    return res;
}
