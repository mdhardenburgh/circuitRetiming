/**
 * Command line args 
 * -i input file name. If not specified default is inputCircuit.txt, at current directory
 * -o output file name. If not specified default is retimedCircuit.txt, at current directory
 * -v turn on verbosity. 
 * -version print version info
 * -h print this help menu again
*/

// retimeCircuit -v  -i inputCircuit.txt -o retimedCircuit.txt

#include <cstdint>
#include <iostream>

#include "retimer.h"
#include "parser.h"
#include "verbosity.h"

void init
(
    std::shared_ptr<Parser> myParser,
    std::shared_ptr<Verbosity> myVerbosity,
    std::shared_ptr<Retimer> myRetimer
)
{
    myVerbosity->vout("Init Parser");
    myParser->setRetimerPtr(myRetimer);
    myParser->setVerbosityPtr(myVerbosity);

    myVerbosity->vout("Init Retimer");
    myRetimer->setVerbosityPtr(myVerbosity);
}

int main(int argc, char* argv[])
{
    std::shared_ptr<Parser> myParser = std::make_shared<Parser>();
    std::shared_ptr<Verbosity> myVerbosity = std::make_shared<Verbosity>();
    std::shared_ptr<Retimer> myRetimer = std::make_shared<Retimer>();
    
    for(int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        if(arg == "-v")
        {
            myVerbosity->enableVerbosity();
        }
        else if(arg == "-i")
            myParser->setInputFile(arg);
        else if(arg == "-o")
            myParser->setOutputFile(arg);
        else if(arg == "-version")
            std::cout<<"Retimer. version 1.0.0 (C) Matthew Hardenburgh. All rights reserved"<<std::endl;
        else if(arg == "-h")
        {
            std::cout<<"-i \"input file name\". If not specified, default is inputCircuit.txt, at current directory"<<std::endl;
            std::cout<<"-i \"output file name\". If not specified, default is retimedCircuit.txt, at current directory"<<std::endl;
            std::cout<<"-v. Turn on verbosity"<<std::endl;
            std::cout<<"-version. Print out version info"<<std::endl;
            std::cout<<"-h. Print out this help menu again"<<std::endl;
            return 0; // Immediately exit
        }
    }

    init(myParser, myVerbosity, myRetimer);
    myParser->parseInput();
    myRetimer->retimeCircuit();
    if(myRetimer->retimeCircuit())
    {
        myRetimer->applyRetimingVector();
        myParser->writeOutput(myRetimer->getRetimedCircuit());
    }
    else
    {
        std::cout<<"retiming failed"<<std::endl;
    }
    return 0;
}