  GNU nano 7.2                                                                               Maze.h                                                                                         #ifndef MAZE_H
#define MAZE_H

#include <vector>

enum Direction { TOP = 0, RIGHT, BOTTOM, LEFT };

struct Cell {
    bool walls[4] = { true, true, true, true }; // top, right, bottom, left
    bool visited = false;
};

class Maze {
public:
    int width;
    int height;
    std::vector<std::vector<Cell>> grid;

    Maze(int w, int h);
    void generate();
};

#endif
