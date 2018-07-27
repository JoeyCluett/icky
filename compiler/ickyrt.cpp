#include <iostream>
#include <vector>
#include <string>

// Icky includes
#include "icky/Icky.h"
#include "icky/IckyRuntime.h"

using namespace std;

int main(int argc, char* argv[]) {
    // turn those pesky char* into std::strings
    vector<string> str_vec;
    for(int i = 0; i < argc; i++)
        str_vec.push_back(argv[i]);

    if(str_vec.at(1) == "-c") { // compile
        
        string filename = str_vec.at(2) + ".icky";
        IckyRuntimeData* ird = new IckyRuntimeData;
        
        IckyAsm::compile(ird, filename);
        IckyAsm::saveRuntimeData(ird, str_vec.at(2) + ".ickyasm");
        delete ird;
    
    } else if(str_vec.at(1) == "-x") { // execute
    
        string filename = str_vec.at(2) + ".ickyasm";
        IckyRuntimeData* ird = new IckyRuntimeData;

        IckyAsm::loadRuntimeData(ird, filename);
        IckyAsm::execute(ird);
        delete ird; // clean up after ourselves because we are not slobs
    
    } else if(str_vec.at(1) == "-xc" || str_vec.at(1) == "-cx") { // compile and execute

        string input_file  = str_vec.at(2) + ".icky";
        string output_file = str_vec.at(2) + ".ickyasm";

        IckyRuntimeData* ird = new IckyRuntimeData;

        IckyAsm::compile(ird, input_file);
        IckyAsm::saveRuntimeData(ird, output_file);
        IckyAsm::execute(ird);

        delete ird;

    } else {
        
        cerr << "Usage:\n  " << argv[0] << " <'-c'|'-x'|'-cx'> <file specifier>\n";
        return 1;

    }

    return 0;

}