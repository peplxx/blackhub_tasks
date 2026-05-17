#include <cstdint>
#include <vector>
#include <iostream>
#include <cassert>

class Solution {
private:
    void sinkIslandFlat(std::vector<uint32_t> &grid, int m, int total_size, int idx) {
        grid[idx] = 0;
        int c = idx % m;

        if (c + 1 < m && grid[idx + 1]) {
            sinkIslandFlat(grid, m, total_size, idx + 1);
        }
        if (idx + m < total_size && grid[idx + m]) {
            sinkIslandFlat(grid, m, total_size, idx + m);
        }
        if (c > 0 && grid[idx - 1]) {
            sinkIslandFlat(grid, m, total_size, idx - 1);
        }
        if (idx - m >= 0 && grid[idx - m]) {
            sinkIslandFlat(grid, m, total_size, idx - m);
        }
    }

public:
    int numIslands(std::vector<uint32_t> grid, int m, int n) {
        int island_count = 0;
        int total_size = n * m;

        for (int i = 0; i < total_size; ++i) {
            if (grid[i] == 0) continue;

            ++island_count;
            sinkIslandFlat(grid, m, total_size, i);
        }

        return island_count;
    }
};

int main() {
    Solution solver;

    // Тест 1
    std::vector<uint32_t> t1 = {
        0,0,1,1,0,0,
        0,1,1,1,0,1,
        0,1,0,0,0,1,
        0,0,0,1,0,1,
        0,1,0,1,0,1,
    };
    assert(solver.numIslands(t1, 6, 5) == 4);

    // Тест 2
    std::vector<uint32_t> t2 = {
        0, 0,
        0, 0
    };
    assert(solver.numIslands(t2, 2, 2) == 0);

    // Тест 3
    std::vector<uint32_t> t3 = {
        1, 1, 1,
        1, 1, 1,
        1, 1, 1
    };
    assert(solver.numIslands(t3, 3, 3) == 1);

    // Тест 4
    std::vector<uint32_t> t4 = {
        1, 1, 1, 1,
        0, 0, 0, 1,
        1, 1, 0, 1,
        1, 1, 1, 1
    };
    assert(solver.numIslands(t4, 4, 4) == 1);

    // Тест 5
    std::vector<uint32_t> t5 = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    assert(solver.numIslands(t5, 4, 4) == 4);

    std::cout << "Все тесты успешно прошли!" << std::endl;
    return 0;
}
