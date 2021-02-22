#include "BasicBlock.h"

class Function
{
public:
    Function (std::string s)
    {
        this->label = s;
    }
    Function (std::string s, BasicBlock* BB)
    {
        this->label = s;
        this->entry = BB;
        this->exit = BB;
        this->constBB = new BasicBlock (0);
    }
    Function (BasicBlock* entry, BasicBlock* exit)
    {
        this->entry = entry;
        this->exit = exit;
    }
    
    void pushBackBB  (BasicBlock* toInsert);
    void pushFrontBB (BasicBlock* toInsert);
    void pushAfterBB (BasicBlock* toInsert, BasicBlock* after);
    
    void pushConstInstruction (Instruction* instr);
    
    
    std::string toString ();
    
    void dotGraph (std::string* basicBlocks, std::string* edges);
    
private:
    BasicBlock* entry;
    BasicBlock* exit;
    BasicBlock* constBB;
    std::string label;
    //std::vector<BasicBlock* > body;
    
    
};
