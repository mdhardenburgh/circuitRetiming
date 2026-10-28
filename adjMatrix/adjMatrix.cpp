#include <cstdint>
#include <vector>
#include <limits>
#include "adjMatrix.h"

AdjMatrix::AdjMatrix()
{

}
AdjMatrix::AdjMatrix(uint32_t totalNodes)
{
    m_totalNodes = totalNodes;
    m_matrix.resize(m_totalNodes);
    for(uint32_t i = 0; i < m_matrix.size(); i++)
    {
        m_matrix[i].resize(totalNodes);
    }

    // negative edges are legit in FW alg, fix
    for(uint32_t row = 0; row < m_totalNodes; row++)
    {
        for(uint32_t col = 0; col < m_totalNodes; col++)
        {
            m_matrix[row][col] = INF;
        }
    }
}
AdjMatrix::~AdjMatrix()
{

}

void AdjMatrix::setEdge(uint32_t u, uint32_t v, int64_t value)
{
    m_matrix[u][v] = value;
}
int64_t AdjMatrix::getEdge(uint32_t u, uint32_t v)
{
    return m_matrix[u][v];
}