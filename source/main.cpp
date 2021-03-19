#include <string>
#include "Parser.h"

std::string fileToCompile;

int main (int argc, char* argv[])
{
    for (int i = 1; i < argc; i++)
    {
        fileToCompile = argv[i];
        
        Parser tmp (fileToCompile);
        tmp.parse ();
        
        tmp.dotGraph (fileToCompile+".smplIR");
    }
}
