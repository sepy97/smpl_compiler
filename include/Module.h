
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
    
//private:
    std::vector<Function* > body;
    
};
