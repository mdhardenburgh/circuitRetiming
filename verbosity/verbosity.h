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

        /**
         * @brief Enable verbosity
         * @return No return.
        */
        void enableVerbosity();
        /**
         * @brief Fetch if verbosity is enabled or not.
         * @return True if enabled, false if not.
        */
        bool getVerbosityStatus();
        /**
         * @brief Like cout except for verbosity.
         * @param myStream: string stream that you want to print.
         * @return No return.
        */
        void vout(const std::stringstream &myStream);
        /**
         * @brief Like cout except for verbosity.
         * @param myString: string that you want to print.
         * @return No return.
        */
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