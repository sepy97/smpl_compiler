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

void Function::pushConstInstruction (Instruction* instr)
{
    this->constBB->body.push_back (instr);
}

std::string Function::toString ()
{
    std::string res = "";
    res += this->label;
    res += "\n";
    int bbCounter = 0;
    if (!constBB->body.empty ())
    {
        res += "BB";
        res += std::to_string (bbCounter);
        bbCounter++;
        res += ":\n";
        res += constBB->toString ();
        res += "########\n";
    }
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

void Function::dotGraph (std::string* basicBlocks, std::string* edges)
{
    *basicBlocks += this->label;
    *basicBlocks += "\n{\n";
    
    std::queue <BasicBlock*> q;
    std::set <BasicBlock*> visited;
    int bbCounter = 0;
    
    if (!constBB->body.empty ())
    {
        *basicBlocks += "bb0";
        //*basicBlocks += std::to_string (constBB->label);
        *basicBlocks += " [shape=record, label= \"<b>BB0|{";
        //*basicBlocks += std::to_string (constBB->label);
        //*basicBlocks += "|{";
        
        constBB->dotGraph (basicBlocks, edges);
        
        *basicBlocks += "}\"]; \n";
        
        *edges += "bb0:s -> bb1:n [label=\"fall-through\"];\n";
        /*
        res += "BB";
        res += std::to_string (bbCounter);
        bbCounter++;
        res += ":\n";
        res += constBB->toString ();
        res += "########\n";*/
    }
    
    if (this->entry)
    {
        q.push (this->entry);
        while (!q.empty ())
        {
                BasicBlock* BB = q.front ();
            
            //std::cout << BB->body.size () << std::endl;
            
                if ((visited.insert (BB)).second == true )
                {
                    *basicBlocks += "bb";
                    *basicBlocks += std::to_string (BB->label);
                    *basicBlocks += " [shape=record, label= \"<b>BB";
                    *basicBlocks += std::to_string (BB->label);
                    *basicBlocks += "|{";
                    
                    BB->dotGraph (basicBlocks, edges);
                    
                    *basicBlocks += "}\"]; \n";
                    
                    for (BasicBlock* succ: BB->successors)
                    {
                        q.push (succ);
                    }
                    
                    for (BasicBlock* pred: BB->predecessors)
                    {
                        
                        *edges += "bb";
                        *edges += std::to_string (pred->label);
                        *edges += ":s -> bb";
                        *edges += std::to_string (BB->label);
                        *edges += ":n ";
                        
                        
            //std::cout << "CFG: " << pred->body.back ()->getOperand1 () << " " << pred->body.back ()->getOperand2 () << " " << BB->body.front ()->getLine () << std::endl;
                        
                        if (pred->body.empty ())
                        {
                            *edges += "[label=\"fall-through\"]";
                        }
                        else
                        {
                            int firstLine = -1;
                            Instruction* first;
                            if (BB->phiInstructions.empty ()) first = BB->body.front ();
                            else first = BB->phiInstructions.front ().first;
                            while (first->getOp () == nop) ++first;
                            
                            if (
                                ( pred->body.back ()->getOp () == op_bne || pred->body.back ()->getOp () == op_beq || pred->body.back ()->getOp () == op_ble || pred->body.back ()->getOp () == op_blt || pred->body.back ()->getOp () == op_bge || pred->body.back ()->getOp () == op_bgt ) &&
                                (pred->body.back ()->getOperand2 () == first->getLine ())
                               )
                            {
                                //std::cout << "CFG: " << pred->body.back ()->getOperand2 () << " " << BB->body.front ()->getLine () << std::endl;
                                *edges += "[label=\"branch\"]";
                            }
                            else if (pred->body.back ()->getOp () == op_bra && pred->body.back ()->getOperand1 () == first->getLine ())
                            {
                                *edges += "[label=\"branch\"]";
                            }
                            else
                            {
                                *edges += "[label=\"fall-through\"]";
                            }
                        }
                        *edges += ";\n";
                            //q.push (succ);
                    }
                    
                }
                q.pop ();
        }
        
        *edges += "\n}";
    }
    else
    {
        //res += "{Empty function}\n";
    }
    
}
