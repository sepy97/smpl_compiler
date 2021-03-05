
#include "Function.h"

class Module
{
public:
    Module () {}
    Module (Function* f)
    {
        body.push_back (f);
    }
    
    std::string toString ();
    void insertFunc (Function* f);
    void dotGraph (std::string* basicBlocks, std::string* edges);
    
//private:
    std::vector<Function* > body;
    
};
