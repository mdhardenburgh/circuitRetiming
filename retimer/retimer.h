#ifndef RETIMER_H
#define RETIMER_H
#include <memory>
#include <string>
#include <vector>
#include <tuple>

#include "verbosity.h"
#include "adjMatrix.h"

class Retimer
{
    public:
        Retimer();
        ~Retimer();
        void setCircuit(std::shared_ptr<AdjMatrix> circuit);
        void setNodeDelays(std::vector<int32_t> nodeDelays){m_nodeDelays = nodeDelays;}
        void setMaxClockCycle(uint32_t maxClockCycle){m_maxClockCycle = maxClockCycle;}
        void setVerbosityPtr(std::shared_ptr<Verbosity> verbosity);
        void setTotalNodes(uint32_t totalNodes){m_totalNodes = totalNodes;}
        std::vector<int32_t> getNodeDelays(){return m_nodeDelays;}
        uint32_t getTotalNodes(){return m_totalNodes;}
        uint32_t getMaxClockCycle(){return m_maxClockCycle;}
        std::shared_ptr<AdjMatrix> getRetimedCircuit(){return m_retimedCircuit;}
        bool retimeCircuit();
        void applyRetimingVector();
        
    private:
        std::shared_ptr<AdjMatrix> m_circuit;
        std::shared_ptr<AdjMatrix> m_retimedCircuit;
        std::shared_ptr<AdjMatrix> m_circuitPrime;
        std::shared_ptr<AdjMatrix> m_shortestPath;
        std::shared_ptr<AdjMatrix> m_wMatrix;
        std::shared_ptr<AdjMatrix> m_dMatrix;
        std::shared_ptr<AdjMatrix> m_constrGraph;
        std::shared_ptr<Verbosity> m_verbosity;
        std::vector<int32_t> m_nodeDelays;
        std::vector<std::tuple<uint32_t, uint32_t, int64_t>> m_systemOfInequalties;
        std::vector<int64_t> possibleClkPeriods;
        std::vector<int64_t> m_retimingVector;
        uint32_t m_maxClockCycle;
        uint32_t m_totalNodes;
        int64_t m_clockPeriod;
        uint32_t m_optimalClockPeriod;

        bool runBellmanford(int64_t period);
        void computeWD();
        void buildSystemOfInequalities();
        void buildClockPeriodVector();
        void buildConstraintGraph();
        int64_t ceilDivide(int64_t a, int64_t b);
        void printMatrix(std::shared_ptr<AdjMatrix> matrix, uint32_t totalNodes);
};
#endif //RETIMER_H