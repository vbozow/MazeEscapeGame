#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include "ssd1306.h"
#include "Maze.h"

#define MAZE_WIDTH 16
#define MAZE_HEIGHT 8

// Simple maze: 1 = wall, 0 = empty space
uint8_t maze[MAZE_HEIGHT][MAZE_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1},
    {1,0,1,1,1,0,1,0,1,0,1,0,1,1,0,1},
    {1,0,1,0,0,0,0,0,1,0,1,0,0,1,0,1},
    {1,0,1,0,1,1,1,1,1,0,1,1,0,1,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1},
    {1,1,1,0,1,0,1,1,1,1,0,1,1,1,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

SSD1306 display;

int playerX = 1;  // Maze coordinates
int playerY = 1;

void drawCell(int x, int y, bool filled) {
    int px = x * 8;
    int py = y * 8;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (filled) {
                display.drawPixel(px + i, py + j);} else {
                // No direct clearPixel, so just skip drawing pixel (leave blank)
            }
        }
    }
}

void drawMaze(const Maze& maze, SSD1306& display, int cellSize) {
    for (int y = 0; y < maze.height; ++y) {
        for (int x = 0; x < maze.width; ++x) {
            int px = x * cellSize;
            int py = y * cellSize;

            if (maze.grid[y][x].walls[TOP]) {
                // draw top wall line
                for (int i = 0; i < cellSize; ++i) {
                    display.drawPixel(px + i, py);
                }
            }
            if (maze.grid[y][x].walls[RIGHT]) {
                // draw right wall line
                for (int i = 0; i < cellSize; ++i) {
                    display.drawPixel(px + cellSize, py + i);
                }
            }
            if (maze.grid[y][x].walls[BOTTOM]) {
                // draw bottom wall line
                for (int i = 0; i < cellSize; ++i) {
                    display.drawPixel(px + i, py + cellSize);
                }
            }
            if (maze.grid[y][x].walls[LEFT]) {
                // draw left wall line
                for (int i = 0; i < cellSize; ++i) {
                    display.drawPixel(px, py + i);
                }
            }
        }
    }
}


int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}
char getch() {
    char c = 0;
    struct termios oldattr, newattr;

    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    read(STDIN_FILENO, &c, 1);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);

    return c;
}
int main()

int mazeWidth = 16;  // Example size, adjust for your display
int mazeHeight = 8;

Maze maze(mazeWidth, mazeHeight);
maze.generate();


{
    if (!display.begin()) {
        std::cerr << "SSD1306 initialization failed" << std::endl;
        return 1;
    }

    drawMaze();
    drawPlayer();
    display.sendBuffer();

    while (true) {
        if (kbhit()) {
            char ch = getch();
            int newX = playerX;
            int newY = playerY;

            if (ch == 'w') newY--;
            else if (ch == 's') newY++;
            else if (ch == 'a') newX--;
            else if (ch == 'd') newX++;
            else if (ch == 'q') break;
if (newX >= 0 && newX < MAZE_WIDTH &&
                newY >= 0 && newY < MAZE_HEIGHT &&
                maze[newY][newX] == 0) {
                playerX = newX;
                playerY = newY;
            }

            drawMaze();
            drawPlayer();
            display.sendBuffer();
        }
        usleep(50 * 1000); // 50 ms delay
    }

    return 0;
}