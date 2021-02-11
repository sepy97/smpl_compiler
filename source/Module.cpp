#include "Module.h"
#include <iostream>

void Module::insertFunc (Function* f)
{
    this->body.push_back (f);
}

std::string Module::toString ()
{
    //std::cout << "Module toString" << std::endl;
    std::string res = "";
    for (Function* f: this->body)
    {
        res += f->toString ();
        //res += "\n";
    }
    
    return res;
}
