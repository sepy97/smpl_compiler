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
    void pushConstInstruction (Instruction* instr);
    
    BasicBlock* getEntry ()
    {
        return this->entry;
    }
    
    std::string toString ();
    void dotGraph (std::string* basicBlocks, std::string* edges);
    bool isVoid = true;
    
/**
*  varTable stores variable id (first) and SSA line number (second)
*/
    std::map <int, int> varTable;

 /**
 *  arrTable stores array id (index of an element), memory offset (first in a pair), and vector of dimensions (second in a pair)
 */
    std::map <int, std::pair <int, std::vector <int> > > arrTable;
    
/**
*  params stores SSA values of transferred parameters
*/
    std::map <int, int> params;
    int returningValue = -1;
    
private:
    BasicBlock* entry;
    BasicBlock* exit;
    BasicBlock* constBB;
    int label;
};
