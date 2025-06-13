  GNU nano 7.2                                                                                                                                Maze.cpp                                                                                                                                          #include "Maze.h"
#include <cstdlib>
#include <ctime>
#include <stack>
#include <utility>
#include <algorithm>

Maze::Maze(int w, int h) : width(w), height(h), grid(h, std::vector<Cell>(w)) {}

void Maze::generate() {
    srand(time(0)); // Random seed

    std::stack<std::pair<int, int>> stack;
    int x = rand() % width;
    int y = rand() % height;
    grid[y][x].visited = true;
    stack.push({x, y});

    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {-1, 0, 1, 0};

    while (!stack.empty()) {
        auto [cx, cy] = stack.top();
        std::vector<int> dirs = {0, 1, 2, 3};
        std::random_shuffle(dirs.begin(), dirs.end());

        bool moved = false;
        for (int dir : dirs) {
            int nx = cx + dx[dir];
            int ny = cy + dy[dir];

            if (nx >= 0 && ny >= 0 && nx < width && ny < height && !grid[ny][nx].visited) {
                grid[cy][cx].walls[dir] = false;
                grid[ny][nx].walls[(dir + 2) % 4] = false;
                grid[ny][nx].visited = true;
                stack.push({nx, ny});
                moved = true;
                break;
            }
        }

        if (!moved) stack.pop();
    }
}
