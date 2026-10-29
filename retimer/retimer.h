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

        /**
         * @brief Set the circuit to be retimed.
         * @param circuit: the circuit to be retimed
         * @return No return.
         * 
        */
        void setCircuit(std::shared_ptr<AdjMatrix> circuit);
        /**
         * @brief Set the node (combinational logic) delay.
         * @param nodeDelays: the combinational node delays. Index 0 is delay for node 0, index 1 is delay for node 1, etc...
         * @return No return.
        */
        void setNodeDelays(std::vector<int32_t> nodeDelays){m_nodeDelays = nodeDelays;}
        /**
         * @brief Set the max number of clock cycle delays (registers) of the circuit.
         * @param maxClockCycle: the max number of clock cycles between combinational logic nodes
         * @return No return.
        */
        void setMaxClockCycle(uint32_t maxClockCycle){m_maxClockCycle = maxClockCycle;}
        /**
         * @brief Set the verbosity pointer so the retimer object can use verbosity.
         * @param verbosity: the verbosity pointer
         * @return No return.
        */
        void setVerbosityPtr(std::shared_ptr<Verbosity> verbosity);
        /**
         * @brief Set the total number of nodes (islands of combinational logic) in the graph.
         * @param totalNodes: the total number of nodes in the graph
         * @return No return.
        */
        void setTotalNodes(uint32_t totalNodes){m_totalNodes = totalNodes;}
        /**
         * @brief Fetch the node (combinational logic) delay. 
         * @return Vector containing the node delays.
         * @details Index 0 is delay for node 0, index 1 is delay for node 1, etc...
        */
        std::vector<int32_t> getNodeDelays(){return m_nodeDelays;}
        /**
         * @brief Fetch the total number of nodes (islands of combinational logic) in the graph.
         * @return The total number of nodes in the graph.
        */
        uint32_t getTotalNodes(){return m_totalNodes;}
        /**
         * @brief Fetch the max number of clock cycle delays (registers) of the circuit.
         * @return The the max number of clock cycle delays (registers) of the circuit.
        */
        uint32_t getMaxClockCycle(){return m_maxClockCycle;}
        /**
         * @brief Get pointer to retimed circuit.
         * @return Pointer to retimed circuit.
        */
        std::shared_ptr<AdjMatrix> getRetimedCircuit(){return m_retimedCircuit;}
        /**
         * @brief Attempt to retime the circuit.
         * @return True if retiming passed, false if it failed.
        */
        bool retimeCircuit();
        /**
         * @brief Apply the retiming vector (if it exists) to a copy of the original graph.
        */
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

        /**
         * @brief Run the Bellman-Ford algorithm using a specified number of clock cycles as a constraint.
         * @param period: clock cycle constraint
         * @return True if retiming was sucessful, false if not.
        */
        bool runBellmanford(int64_t period);

        /**
         * @brief Compute the W and D matrices using the Floyd-Warshall algorithm
         * @return No return.
        */
        void computeWD();
        /**
         * @brief Build the system of inequalities. Start with the feasibility
         * constraints, then add the clock period constraints.
         * @return No return.
        */
        void buildSystemOfInequalities();
        /**
         * @brief Build the vector of possible clock periods to binary search through.
         * @return No return.
        */
        void buildClockPeriodVector();
        /**
         * @brief Build the constraint graph and add the totalNodes + 1 node.
         * @return No return. 
        */
        void buildConstraintGraph();
        /**
         * @brief Divide a and b and take the ceiling.
         * @param a: dividend
         * @param b: divsior
         * @return Ceiling of a/b.
        */
        int64_t ceilDivide(int64_t a, int64_t b);
        /**
         * @brief Helper function to print an adjacency matrix. Most of the
         * intermediate steps are stored as an adjacency matrix.
         * @param matrix: matrix you want to print
         * @param totalNodes: total nodes in the matrix
         * @return No Return.
        */
        void printMatrix(std::shared_ptr<AdjMatrix> matrix, uint32_t totalNodes);
};
#endif //RETIMER_H