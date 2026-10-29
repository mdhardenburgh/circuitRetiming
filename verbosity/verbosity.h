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

        // Overload << operator
        template<typename T>
        Verbosity& operator<<(const T& value)
        {
            if (verbose)
            {
                std::cout << value;
            }
            return *this;

        }

        // Support std::endl
        Verbosity& operator<<(std::ostream& (*manip)(std::ostream&))
        {
            if (verbose)
            {
                manip(std::cout);
            }
            return *this;
        }

    private:
        bool verbose = false;
};
#endif // VERBOSITY_H