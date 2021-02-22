
#include "Function.h"

class Module
{
public:
    Module () {}
    Module (Function* f)
    {
        body.push_back (f);
    }
    
    void insertFunc (Function* f);
    
    std::string toString ();
    
    void dotGraph (std::string* basicBlocks, std::string* edges);
    
//private:
    std::vector<Function* > body;
    
};
