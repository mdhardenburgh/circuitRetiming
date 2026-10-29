#include "parser.h"
#include "retimer.h"
#include "adjMatrix.h"
#include <fstream>
#include <regex>
#include <string>
#include <iostream>
#include <tuple>
#include <iomanip>
#include <vector>

Parser::Parser()
{
    m_inputFileName = "inputCircuit.txt";
    m_outputFileName = "retimedCircuit";

    totalNodesRegexPattern = R"(TotalNodes\s*=\s*(\d+);)";
    nodeDelaysRegexPattern = R"(NodeDelays\s*=\s*(\d+(?:\s*,\s*\d+)*)\s*;)";
    edgeDelayRegexPattern = R"(Edge(\d+),(\d+)\s*=\s*(\d+);)";
    maxClockCycleRegexPattern = R"(MaxClockCycle\s*=\s*(\d+);)";
}

Parser::~Parser()
{

}
void Parser::setInputFile(std::string inputFileName)
{
    m_inputFileName = inputFileName;
}
void Parser::setOutputFile(std::string outputFileName)
{
    m_outputFileName = outputFileName;
}
void Parser::setRetimerPtr(std::shared_ptr<Retimer> retimerPtr)
{
    m_retimerPtr = retimerPtr;
}
void Parser::parseInput()
{
    uint32_t totaParsedlNodes;
    uint32_t parsedMaxClockCycle;
    std::vector<int32_t> parsedNodeDelays;
    std::vector<std::tuple<uint32_t, uint32_t, uint32_t>> parsedEdgeDelays;

    std::ifstream file(m_inputFileName);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file\n";
        return;
    }

    // compile regex patterns
    std::regex totalNodes(totalNodesRegexPattern);
    std::regex nodeDelays(nodeDelaysRegexPattern);
    std::regex edgeDelay(edgeDelayRegexPattern);
    std::regex maxClockCycle(maxClockCycleRegexPattern);

    std::string line;
    std::smatch match;


    /**
     * Search for specific patterns, ex:
     * TotalNodes=4;
     * NodeDelays=1,1,2,2;
     * Edge1,3=1;
     * MaxClockCycle=3;
    */
    while (std::getline(file, line))
    {
        if(std::regex_search(line, match, totalNodes))
        {
            totaParsedlNodes = static_cast<uint32_t>(std::stoul(match[1].str()));
            (*m_verbosityPtr)<< "TotalNodes: " << totaParsedlNodes << std::endl;
        }
        else if(std::regex_search(line, match, nodeDelays))
        {
            std::regex numberPattern(R"(\d+)");
            std::string numbers = match[1].str();

            auto begin = std::sregex_iterator(numbers.begin(), numbers.end(), numberPattern);
            auto end = std::sregex_iterator();

            (*m_verbosityPtr)<<"node delays"<<std::endl;
            for (auto it = begin; it != end; ++it)
            {
                uint32_t value = static_cast<uint32_t>(std::stoul(it->str()));
                parsedNodeDelays.push_back(value);
                (*m_verbosityPtr)<< value << std::endl;
            }
        }
        else if(std::regex_search(line, match, edgeDelay))
        {   
            parsedEdgeDelays.push_back(std::make_tuple
            (
                static_cast<uint32_t>(std::stoul(match[1].str())),
                static_cast<uint32_t>(std::stoul(match[2].str())),
                static_cast<uint32_t>(std::stoul(match[3].str()))
            ));
        }
        else if(std::regex_search(line, match, maxClockCycle))
        {
            parsedMaxClockCycle = static_cast<uint32_t>(std::stoul(match[1].str()));
            (*m_verbosityPtr)<< "parsedMaxClockCycle: " << parsedMaxClockCycle << std::endl;
        }
        else
        {
            
        }
    }

    (*m_verbosityPtr)<<"Done Parsing"<<std::endl;
    (*m_verbosityPtr)<< "TotalNodes: " << totaParsedlNodes << std::endl;
    for(int i = 0; i < parsedNodeDelays.size(); i++)
    {
        (*m_verbosityPtr)<<"parsed node delay "<<i<<": "<<parsedNodeDelays.at(i)<<std::endl;
    }
    for(int i = 0; i < parsedEdgeDelays.size(); i++)
    {
        (*m_verbosityPtr)<<"u: "<<std::get<0>(parsedEdgeDelays.at(i))
        <<" v: "<<std::get<1>(parsedEdgeDelays.at(i))
        <<" delay: "<<std::get<2>(parsedEdgeDelays.at(i))<<std::endl;
    }
    (*m_verbosityPtr) << "MaxClockCycle: "<<parsedMaxClockCycle<<std::endl;

    // create the adjacency matrix representing the 
    std::shared_ptr<AdjMatrix> myMatrix = std::make_shared<AdjMatrix>(totaParsedlNodes);
    // Now we put the edges in the matrix
    for(int i = 0; i < parsedEdgeDelays.size(); i++)
    {
        myMatrix->setEdge
        (
            std::get<0>(parsedEdgeDelays.at(i))-1, //convert 1-indexed to 0-indexed
            std::get<1>(parsedEdgeDelays.at(i))-1, //convert 1-indexed to 0-indexed
            std::get<2>(parsedEdgeDelays.at(i))
        );
    }
    // pass everything to retimer object
    m_retimerPtr->setMaxClockCycle(parsedMaxClockCycle);
    m_retimerPtr->setNodeDelays(parsedNodeDelays);
    m_retimerPtr->setCircuit(myMatrix);
    m_retimerPtr->setTotalNodes(totaParsedlNodes);
}

void Parser::writeOutput(std::shared_ptr<AdjMatrix> retimedCircuit)
{
    std::ofstream outputFile(m_outputFileName+".txt", std::ios::out | std::ios::trunc);
    std::vector<int32_t> nodeDelays = m_retimerPtr->getNodeDelays();

    std::ofstream outputFileGraph(m_outputFileName+".graph", std::ios::out | std::ios::trunc);

    if (outputFileGraph.is_open())
    {
        outputFileGraph<<"retimed circuit"<<std::endl;
        for (uint32_t u = 0; u < m_retimerPtr->getTotalNodes(); u++)
        {
            for (uint32_t v = 0; v < m_retimerPtr->getTotalNodes(); v++)
            {
                if(retimedCircuit->getEdge(u, v) == AdjMatrix::INF)
                {
                    outputFileGraph<<std::setw(6)<<"INF";
                }
                else
                {
                    outputFileGraph<<std::setw(6)<<retimedCircuit->getEdge(u, v);
                }
            }    
            outputFileGraph<<std::endl;
        }
    }

    if(outputFile.is_open())
    {
        outputFile<<"TotalNodes = "<<m_retimerPtr->getTotalNodes()<<";"<<std::endl;
        outputFile<<"NodeDelays = ";
        for(uint32_t i = 0; i < nodeDelays.size(); i++)
        {
            if(i == nodeDelays.size()-1)
            {
                outputFile<<nodeDelays.at(i)<<";"<<std::endl;
            }
            else
            {
                outputFile<<nodeDelays.at(i)<<", ";
            }
        }

        for (uint32_t u = 0; u < m_retimerPtr->getTotalNodes(); u++)
        {
            for (uint32_t v = 0; v < m_retimerPtr->getTotalNodes(); v++)
            {
                int64_t edgeWeight = retimedCircuit->getEdge(u, v);
                if(edgeWeight != AdjMatrix::INF)
                {
                    outputFile<< "Edge"<<u<<","<<v<<" = "<<edgeWeight<<";"<<std::endl;
                }
            }
        }

        outputFile<<"MaxClockCycle = "<<m_retimerPtr->getMaxClockCycle()<<";"<<std::endl;
    }
    outputFile.close();
}