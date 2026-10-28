#ifndef ADJMATRIX_H
#define ADJMATRIX_H
#include <cstdint>
#include <vector>
#include <limits>

class AdjMatrix
{
    public:
        AdjMatrix();
        AdjMatrix(uint32_t totalNodes);
        ~AdjMatrix();

        void setEdge(uint32_t u, uint32_t v, int64_t value);
        int64_t getEdge(uint32_t u, uint32_t v);
        inline static constexpr int64_t INF = std::numeric_limits<int64_t>::max();
    private:
        uint32_t m_totalNodes;
        std::vector<std::vector<int64_t>> m_matrix;
};
#endif //ADJMATRIX_H