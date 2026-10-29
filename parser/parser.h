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

        void setInputFile(std::string inputFileName);
        void setOutputFile(std::string outputFileName);
        void setRetimerPtr(std::shared_ptr<Retimer> retimerPtr);
        void setVerbosityPtr(std::shared_ptr<Verbosity> Verbosity){m_verbosityPtr = Verbosity;}
        // use regex to parse file
        void parseInput();
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


        void parseFor_totalNodes();
        void parseFor_nodeDelays();
        void parseFor_edgeDelay();
        void parseFor_maxClockCycle();
};
#endif //PARSER_H 