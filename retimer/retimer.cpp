#include "retimer.h"
#include "verbosity.h"
#include <algorithm>
#include <vector>
#include <tuple>
#include <cstdint>
#include <iomanip>

Retimer::Retimer()
{
    
}
Retimer::~Retimer()
{


}
int64_t Retimer::ceilDivide(int64_t a, int64_t b)
{
    return a/b + (a%b > 0 ? 1:0);
}
void Retimer::setVerbosityPtr(std::shared_ptr<Verbosity> verbosity)
{
    m_verbosity = verbosity;
}

void Retimer::setCircuit(std::shared_ptr<AdjMatrix> circuit)
{
    m_circuit = circuit;
}

bool Retimer::retimeCircuit()
{
    // check for negative combinational delay
    computeWD();
    buildClockPeriodVector();

    (*m_verbosity)<<"begin binary search"<<std::endl;
    // begin binary search
    uint32_t low = 0; 
    uint32_t high = possibleClkPeriods.size(); // first feasible candidate
    while (low < high)
    {
        uint32_t middle = low + (high - low) / 2;
        int32_t testPeriod = possibleClkPeriods.at(middle);

        (*m_verbosity)<<"test period: "<<testPeriod<<std::endl;
        // if clock period is feasible
        if(runBellmanford(testPeriod))
        {
            // if feasible lets try a smaller clock period
            high = middle;
        }
        else
        {
            low = middle + 1;
        }
    }
    if(low == possibleClkPeriods.size())
    {
        (*m_verbosity)<<"No feasible retiming period was found"<<std::endl;
        m_retimingVector.clear();
        return false;
    }

    // Minimum feasible clock period
    int32_t optimalPeriod = possibleClkPeriods.at(low);
    // Check against the maximum allowed clock period
    if(optimalPeriod > m_maxClockCycle)
    {
        (*m_verbosity) << "No feasible retiming meets MaxClockCycle"<< std::endl;
        m_retimingVector.clear();
        return false;
    }
    m_optimalClockPeriod = optimalPeriod;
    return runBellmanford(optimalPeriod);
}

void Retimer::computeWD()
{
    uint32_t dmax = *std::max_element(m_nodeDelays.begin(),m_nodeDelays.end());
    int64_t M = m_totalNodes * dmax;
    m_shortestPath = std::make_shared<AdjMatrix>(m_totalNodes);
    m_circuitPrime = std::make_shared<AdjMatrix>(m_totalNodes);
    m_wMatrix = std::make_shared<AdjMatrix>(m_totalNodes);
    m_dMatrix = std::make_shared<AdjMatrix>(m_totalNodes);

    (*m_verbosity)<<"original matrix graph G"<<std::endl;
    printMatrix(m_circuit, m_totalNodes);

    // compute G'
    for (uint32_t u = 0; u < m_totalNodes; u++)
    {
        for (uint32_t v = 0; v < m_totalNodes; v++)
        {
            int64_t origWeight = m_circuit->getEdge(u, v);

            // AdjMatrix::INF means the edge does not exist in the original graph
            if (origWeight == AdjMatrix::INF)
            {
                continue;
            }
            int64_t newWeight = M * origWeight - m_nodeDelays[u];
            m_circuitPrime->setEdge(u, v, newWeight);
        }
    }

    (*m_verbosity)<<"circuit G' after applying newWeight = M * origWeight - m_nodeDelays[u]"<<std::endl;
    printMatrix(m_circuitPrime, m_totalNodes);

    // save G` and copy contents to new adj matrix
    for (uint32_t u = 0; u < m_totalNodes; u++)
    {
        for (uint32_t v = 0; v < m_totalNodes; v++)
        {
            m_shortestPath->setEdge(u, v, m_circuitPrime->getEdge(u, v));
        }
    }

    /**
     * Compute the shortest path S'<u,v> using Floyd-Warshall
     * u is starting node, v is destination, k is intermediate nodes
     * would traveling from u to v through k be shorter than the shortest path we already know?
     * if so, update
    */
    for(uint32_t k = 0; k < m_totalNodes; k++)
    {
        for(uint32_t u = 0; u < m_totalNodes; u++)
        {
            for(uint32_t v = 0; v < m_totalNodes; v++)
            {
                if (m_shortestPath->getEdge(u,k) != AdjMatrix::INF && m_shortestPath->getEdge(k,v) != AdjMatrix::INF)
                {
                    m_shortestPath->setEdge(u, v, std::min(m_shortestPath->getEdge(u,v), m_shortestPath->getEdge(u,k) + m_shortestPath->getEdge(k,v)));
                }
            }
        }
    }
    (*m_verbosity)<<"S'<u,v> matrix after Floyd-Warshall"<<std::endl;
    printMatrix(m_shortestPath, m_totalNodes);

    /**
     * Check the shortest-path distance from node i back to itself. Verify
     * there are no negative cycles
    */
    for (uint32_t i = 0; i < m_totalNodes; i++)
    {
        if (m_shortestPath->getEdge(i, i) < 0)
        {
            (*m_verbosity)<<"negative cycle"<<std::endl;
        }
    }

    /**
     * set W and D matricies.
    */
    for (uint32_t u = 0; u < m_totalNodes; ++u)
    {
        for (uint32_t v = 0; v < m_totalNodes; ++v)
        {
            if (u == v)
            {
                m_wMatrix->setEdge(u, v, 0);
                m_dMatrix->setEdge(u, v, m_nodeDelays[u]);
            }
            else if (m_shortestPath->getEdge(u,v) != AdjMatrix::INF)
            {
                /*
                m_W[u][v] = ceilDivide(m_shortestPath[u][v], M);
                m_D[u][v] = M * m_W[u][v] - distance[u][v] + m_nodeDelays[v];
                if (m_W[u][v] < 0)
                    throw std::runtime_error("Invalid W matrix: negative register count");
                */
                m_wMatrix->setEdge(u, v, ceilDivide(m_shortestPath->getEdge(u, v), M));
                m_dMatrix->setEdge(u, v, M * m_wMatrix->getEdge(u, v)-m_shortestPath->getEdge(u, v) + m_nodeDelays.at(v));
                if (m_wMatrix->getEdge(u, v) < 0)
                    (*m_verbosity)<<"negative register count"<<std::endl;
            }
        }
    }
    (*m_verbosity)<<"W Matrix"<<std::endl;
    printMatrix(m_wMatrix, m_totalNodes);
    (*m_verbosity)<<"D Matrix"<<std::endl;
    printMatrix(m_dMatrix, m_totalNodes);

    // print out W and D
}

void Retimer::buildSystemOfInequalities()
{
    (*m_verbosity)<<"build system of inequalities"<<std::endl;
    (*m_verbosity)<<"begin with feasability constraints"<<std::endl;
    m_systemOfInequalties.clear();
    //feasability constraints from OG circuit graph
    for(uint32_t u = 0; u < m_totalNodes; u++)
    {
        for(uint32_t v = 0; v < m_totalNodes; v++)
        {
            if(m_circuit->getEdge(u,v)!=AdjMatrix::INF)
            {
                m_systemOfInequalties.push_back(std::make_tuple
                (
                    u, v, m_circuit->getEdge(u, v)
                ));
            }
        }
    }
    (*m_verbosity)<<"after feasability constraints are added"<<std::endl;
    for (const auto& [u, v, weight] : m_systemOfInequalties)
    {
        (*m_verbosity) << "r(" << u <<") - r("<<v<<") <= "<<weight<<std::endl;
    }
    // clock period constraints from W and D matricies
    for(uint32_t u = 0; u < m_totalNodes; u++)
    {
        for(uint32_t v = 0; v < m_totalNodes; v++)
        {
            if(m_dMatrix->getEdge(u,v) > m_clockPeriod && m_wMatrix->getEdge(u,v)!=AdjMatrix::INF)
            {
                m_systemOfInequalties.push_back(std::make_tuple
                (
                    u, v, m_wMatrix->getEdge(u, v)-1
                ));
            }
        }
    }

    (*m_verbosity)<<"after feasability and clock period constraints are added"<<std::endl;
    for (const auto& [u, v, weight] : m_systemOfInequalties)
    {
        (*m_verbosity) << "r(" << u <<") - r("<<v<<") <= "<<weight<<std::endl;
    }
}

void Retimer::buildClockPeriodVector()
{
    possibleClkPeriods.clear();
    for (uint32_t u = 0; u < m_totalNodes; u++)
    {
        for (uint32_t v = 0; v < m_totalNodes; v++)
        {
            if(m_dMatrix->getEdge(u, v) != AdjMatrix::INF)
                possibleClkPeriods.push_back(m_dMatrix->getEdge(u, v));
        }
    }
    std::sort(possibleClkPeriods.begin(), possibleClkPeriods.end());
    // delete duplicates
    possibleClkPeriods.erase(std::unique(possibleClkPeriods.begin(), possibleClkPeriods.end()),possibleClkPeriods.end());

    (*m_verbosity)<<"possible clock periods: ";
    for(int i = 0; i < possibleClkPeriods.size(); i++)
    {
        (*m_verbosity)<<possibleClkPeriods.at(i)<<", ";
    }
    (*m_verbosity)<<std::endl;
}

void Retimer::buildConstraintGraph()
{
    (*m_verbosity)<<"building constraint graph"<<std::endl;
    m_constrGraph = std::make_shared<AdjMatrix>(m_totalNodes + 1);

    //set constraint graph values from vertex j to vertex i
    for(uint32_t i = 0; i < m_systemOfInequalties.size(); i++)
    {
        uint32_t i_ = std::get<0>(m_systemOfInequalties.at(i));
        uint32_t j = std::get<1>(m_systemOfInequalties.at(i));
        int32_t k = std::get<2>(m_systemOfInequalties.at(i));
        int64_t existing = m_constrGraph->getEdge(j, i_);


        if(existing == AdjMatrix::INF || k < existing)
        {
            m_constrGraph->setEdge(j, i_, k);
        }
    }

    // for the extra or origin node, set all edge weights to zero from origin to every other node
    for(uint32_t i = 0; i < m_totalNodes+1 ; i++)
    {
        m_constrGraph->setEdge
        (
            m_totalNodes, //j
            i, //i
            0  //k
        );
    }

    (*m_verbosity)<<"constraint graph"<<std::endl;
    printMatrix(m_constrGraph, m_totalNodes+1);
}

bool Retimer::runBellmanford(int64_t period)
{
    (*m_verbosity)<<"run Bellman-Ford algorithm"<<std::endl;
    
    // Set the clock period being tested
    m_clockPeriod = period;

    // Build the system of inequalities and constraint graph
    buildSystemOfInequalities();
    buildConstraintGraph();

    // Include the extra super-source vertex
    uint32_t totalVertices = m_totalNodes + 1;
    uint32_t source = m_totalNodes;

    //const int64_t INF = std::numeric_limits<int64_t>::max() / 4;

    // Initialize distance vector
    std::vector<int64_t> distance(totalVertices, AdjMatrix::INF);

    // Distance from source to itself is zero
    distance[source] = 0;

    // Step 1: Relax all edges V-1 times
    for(uint32_t i = 0; i < totalVertices - 1; i++)
    {
        bool updated = false;

        for(uint32_t u = 0; u < totalVertices; u++)
        {
            if(distance[u] == AdjMatrix::INF)
                continue;

            for(uint32_t v = 0; v < totalVertices; v++)
            {
                int64_t weight = m_constrGraph->getEdge(u,v);

                // Skip nonexistent edges
                if(weight == AdjMatrix::INF)
                    continue;

                // Relax the edge, most important part
                if(distance[u] + weight < distance[v])
                {
                    distance[v] = distance[u] + weight;
                    updated = true;
                }
            }
        }

        // Stop if no distances changed
        if(!updated)
            break;
    }

    // Step 2: Check for negative-weight cycles
    for(uint32_t u = 0; u < totalVertices; u++)
    {
        if(distance[u] == AdjMatrix::INF)
            continue;

        for(uint32_t v = 0; v < totalVertices; v++)
        {
            int64_t weight = m_constrGraph->getEdge(u,v);
            if(weight == AdjMatrix::INF)
                continue;

            if(distance[u] + weight < distance[v])
            {
                // Negative cycle detected
                (*m_verbosity)<<"Negative cycle detected"<<std::endl;
                return false;
            }
        }
    }
    // Step 3: Save the retiming vector
    m_retimingVector.assign(distance.begin(),distance.begin() + m_totalNodes);

    (*m_verbosity)<<"retiming vector"<<std::endl;

    (*m_verbosity)<<"R<";
    for(int i = 0; i < m_retimingVector.size(); i++)
    {
        (*m_verbosity)<<m_retimingVector.at(i)<<", ";
    }
    (*m_verbosity)<<">"<<std::endl;

    return true;
}

void Retimer::applyRetimingVector()
{
    m_retimedCircuit = std::make_shared<AdjMatrix>(m_totalNodes);

    /**
     * apply retiming vector
     * w_r(u, v) = w(u, v) + r(v) - r(u)
    */
    for (uint32_t u = 0; u < m_totalNodes; u++)
    {
        for (uint32_t v = 0; v < m_totalNodes; v++)
        {
            int64_t origWeight = m_circuit->getEdge(u, v);
            int64_t r_u = m_retimingVector[u];
            int64_t r_v = m_retimingVector[v];

            // AdjMatrix::INF means the edge does not exist in the original graph
            if (origWeight == AdjMatrix::INF)
            {
                continue;
            }
            else
            {
                m_retimedCircuit->setEdge(u, v, origWeight + (r_v - r_u));
            }
        }
    }
    (*m_verbosity)<<"optimal clock period is: "<<m_optimalClockPeriod<<std::endl;
    (*m_verbosity)<<"Retimed Circuit:"<<std::endl;
    printMatrix(m_retimedCircuit, m_totalNodes);
}

void Retimer::printMatrix(std::shared_ptr<AdjMatrix> matrix, uint32_t totalNodes)
{
    for (uint32_t u = 0; u < totalNodes; u++)
    {
        for (uint32_t v = 0; v < totalNodes; v++)
        {
            if(matrix->getEdge(u, v) == AdjMatrix::INF)
            {
                (*m_verbosity)<<std::setw(6)<<"INF";
            }
            else
            {
                (*m_verbosity)<<std::setw(6)<<matrix->getEdge(u, v);
            }
        }    
        (*m_verbosity)<<std::endl;
    }
    (*m_verbosity)<<std::endl;
}