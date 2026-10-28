#ifndef VERBOSITY_H
#define VERBOSITY_H
#include <string>
#include <sstream>
#include <iostream>

class Verbosity
{
    public:
        Verbosity();
        ~Verbosity();

        void enableVerbosity();
        bool getVerbosityStatus();

        void vout(const std::stringstream &myStream);
        void vout(const std::string &myString);

    private:
        bool verbose = false;
};
#endif // VERBOSITY_H