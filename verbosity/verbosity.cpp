#include <string>
#include <sstream>
#include <iostream>

#include "verbosity.h"

Verbosity::Verbosity()
{

}

Verbosity::~Verbosity()
{

}

void Verbosity::enableVerbosity()
{
    verbose = true;
}

bool Verbosity::getVerbosityStatus()
{
    return verbose;
}

void Verbosity::vout(const std::stringstream &myStream)
{
    if(verbose)
    {
        std::cout<<myStream.str()<<std::endl;
    }
}

void Verbosity::vout(const std::string &myString)
{
    if(verbose)
    {
        std::cout<<myString<<std::endl;
    }
}