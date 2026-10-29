#ifndef PARSER_H
#define PARSER_H
#include <cstdint>
#include <string>
#include <memory>
#include <regex>

#include "verbosity.h"
#include "retimer.h"
#include "adjMatrix.h"
class Parser
{
    public:
        Parser();
        ~Parser();

        /**
         * @brief Set custom input file.
         * @param inputFileName: custom input file name
         * @return No return.
        */
        void setInputFile(std::string inputFileName);
        /**
         * @brief Set custom output file name.
         * @param outputFileName: custom output file name
         * @return No return.
        */
        void setOutputFile(std::string outputFileName);
        /**
         * @brief Set the retiming object pointer so the two objects can communicate.
         * @param retimerPtr: pointer to the retimer object
         * @return No return.
        */
        void setRetimerPtr(std::shared_ptr<Retimer> retimerPtr);
        /**
         * @brief Set the verbosity object pointer so that parser and verbosity can communicate.
         * @param Verbosity: pointer to the verbosity object
         * @return No return.
        */
        void setVerbosityPtr(std::shared_ptr<Verbosity> Verbosity){m_verbosityPtr = Verbosity;}
        /**
         * @brief Use regex to parse the input file into the retimer object.
         * @return No return.
        */
        void parseInput();
        /**
         * @brief Write to output file
         * @param retimedCircuit: pointer to retimed circuit. 
         * @return No return.
        */
        void writeOutput(std::shared_ptr<AdjMatrix> retimedCircuit);
    
    private:
        std::string m_inputFileName;
        std::string m_outputFileName;
        std::shared_ptr<Retimer> m_retimerPtr;
        std::shared_ptr<Verbosity> m_verbosityPtr;
        std::string totalNodesRegexPattern;
        std::string nodeDelaysRegexPattern;
        std::string edgeDelayRegexPattern;
        std::string maxClockCycleRegexPattern;

};
#endif //PARSER_H 