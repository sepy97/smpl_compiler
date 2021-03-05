#include "Instruction.h"
#include <vector>

class BasicBlock
{
public:
    BasicBlock (int label)
    {
        this->label = label;
    }
    
    BasicBlock (Instruction* i, int label)
    {
        this->body = std::vector<Instruction* > {i};
        this->label = label;
    }
    
    void addPred (BasicBlock* pred);
    void addSucc (BasicBlock* succ);
    
    void pushInstruction (Instruction* toInsert);
    bool pushAfter (Instruction* toInsert, Instruction* after);
    
    std::string toString ();
    
    void dotGraph (std::string* basicBlocks, std::string* edges);
    
    std::vector<BasicBlock* > predecessors;
    std::vector<BasicBlock* > successors;
    
    int label;
//private:
    std::vector<Instruction* > body;
    std::vector< std::pair <Instruction*, int > > phiInstructions;  
    
};
