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

void Module::dotGraph (std::string* basicBlocks, std::string* edges)
{
    *basicBlocks += "digraph \n{\n";
    for (Function* f: this->body)
    {
        //*basicBlocks += "digraph ";
        f->dotGraph (basicBlocks, edges);
    }
    *edges += "}\n";
}
