#include "Instruction.h"
#include <vector>

class BasicBlock
{
public:
    BasicBlock () {}
    BasicBlock (Instruction* i)
    {
        this->body = std::vector<Instruction* > {i};
    }
    
    void addPred (BasicBlock* pred);
    void addSucc (BasicBlock* succ);
    
    /*BasicBlock (Instruction* entry, Instruction* exit)
    {
        this->entry = entry;
        this->exit = exit;
    }*/
    
    void pushInstruction (Instruction* toInsert);
    bool pushAfter (Instruction* toInsert, Instruction* after);
    
    std::string toString ();
    
    /*Instruction* entry;
    Instruction* exit;*/
    std::vector<BasicBlock* > predecessors;
    std::vector<BasicBlock* > successors;
    
//private:
    std::vector<Instruction* > body;
    
};
