#include "BasicBlock.h"
#include <map>

class Function
{
public:
    Function (int lbl)
    {
        this->label = lbl;
    }
    Function (int lbl, std::map <int, int> params, BasicBlock* BB)
    {
        this->label = lbl;
        this->entry = BB;
        this->exit = BB;
        this->params = params;
        this->constBB = new BasicBlock (0);
        for (auto& p : params)
        {
            std::cout << "function init : " << p.first << " : " << p.second << std::endl;
            varTable [p.first] = -2;
            this->params [p.first] = p.second;
        }
    }
    Function (BasicBlock* entry, BasicBlock* exit)
    {
        this->entry = entry;
        this->exit = exit;
    }
    
    void pushBackBB  (BasicBlock* toInsert);
    void pushFrontBB (BasicBlock* toInsert);
    void pushAfterBB (BasicBlock* toInsert, BasicBlock* after);
    
    BasicBlock* getEntry ()
    {
        return this->entry;
    }
    
    void pushConstInstruction (Instruction* instr);
    
    std::string toString ();
    
    void dotGraph (std::string* basicBlocks, std::string* edges);
    
    bool isVoid = true;
    
/**
*  varTable stores variable id (first) and SSA line number (second)
*/
    std::map <int, int> varTable;

/**
*  params stores SSA values of transferred parameters
*/
    std::map <int, int> params;
    
    int returningValue;
    
private:
    BasicBlock* entry;
    BasicBlock* exit;
    BasicBlock* constBB;
    int label;
    //std::vector<BasicBlock* > body;
    
    
};
