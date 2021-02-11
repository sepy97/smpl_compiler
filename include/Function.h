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
    }
    Function (BasicBlock* entry, BasicBlock* exit)
    {
        this->entry = entry;
        this->exit = exit;
    }
    
    void pushBackBB  (BasicBlock* toInsert);
    void pushFrontBB (BasicBlock* toInsert);
    void pushAfterBB (BasicBlock* toInsert, BasicBlock* after);
    
    std::string toString ();
    
private:
    BasicBlock* entry;
    BasicBlock* exit;
    std::string label;
    //std::vector<BasicBlock* > body;
    
    
};
