



#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctime>
#include <thread>
#include <wiringPi.h>
#include "ssd1306.h"
#include "Maze.h"

#define MAZE_WIDTH 16
#define MAZE_HEIGHT 8
#define CELL_SIZE 8
#define UP_BUTTON    17
#define DOWN_BUTTON  27
#define LEFT_BUTTON  22
#define RIGHT_BUTTON 23
#define BUZZER_PIN   18

SSD1306 display;
int playerX = 0, playerY = 0;
int goalX = 0, goalY = 0;
bool gameRunning = true;

void drawPlayer() {
    int px = playerX * CELL_SIZE + 2;
    int py = playerY * CELL_SIZE + 2;
    for (int dx = 0; dx < 4; dx++) {
        for (int dy = 0; dy < 4; dy++) {
            display.drawPixel(px + dx, py + dy);
        }
    }
}

void drawMaze(const Maze& maze, SSD1306& display, int cellSize) {
    for (int y = 0; y < maze.height; ++y) {
        for (int x = 0; x < maze.width; ++x) {
            int px = x * cellSize;
            int py = y * cellSize;

            if (maze.grid[y][x].walls[TOP])
                for (int i = 0; i < cellSize; ++i) display.drawPixel(px + i, py);

            if (maze.grid[y][x].walls[RIGHT])
                for (int i = 0; i < cellSize; ++i) display.drawPixel(px + cellSize, py + i);

            if (maze.grid[y][x].walls[BOTTOM])
                for (int i = 0; i < cellSize; ++i) display.drawPixel(px + i, py + cellSize);

            if (maze.grid[y][x].walls[LEFT])
                for (int i = 0; i < cellSize; ++i) display.drawPixel(px, py + i);
        }
    }

    int gx = goalX * cellSize;
    int gy = goalY * cellSize;
    for (int i = 2; i < cellSize - 2; ++i) {
        display.drawPixel(gx + i, gy + i);
        display.drawPixel(gx + i, gy + (cellSize - i - 1));
    }
}

void drawYouWin(SSD1306& display) {
    int x = 10;
    int y = 20;
    // Pixel text: "YOU WIN!" (simplified)
    for (int i = 0; i < 50; ++i) display.drawPixel(x + i, y); // underline bar
}

void showCountdown() {
    for (int i = 3; i > 0; --i) {
        display.clear();
        for (int x = 60; x < 68; ++x)
            for (int y = 20; y < 28; ++y)
                display.drawPixel(x, y); // Block
        display.sendBuffer();
        sleep(1);
    }
    display.clear();
    display.sendBuffer();
}

void gameTimer() {
    int seconds = 0;
    while (gameRunning) {
        printf("Time: %d sec\n", seconds++);
        sleep(1);
    }
}

int main() {
    srand(time(0));

    Maze maze(MAZE_WIDTH, MAZE_HEIGHT);
    maze.generate();

    playerX = 0;
    playerY = 0;

    for (int y = MAZE_HEIGHT - 1; y >= 0; --y) {
        for (int x = MAZE_WIDTH - 1; x >= 0; --x) {
            if (maze.grid[y][x].visited) {
                goalX = x;
                goalY = y;
                goto found;
            }
        }
    }
found:

    if (!display.begin()) {
        std::cerr << "SSD1306 init failed!" << std::endl;
        return 1;
    }

    wiringPiSetupGpio();
    pinMode(UP_BUTTON, INPUT);
    pinMode(DOWN_BUTTON, INPUT);
    pinMode(LEFT_BUTTON, INPUT);
    pinMode(RIGHT_BUTTON, INPUT);
    pullUpDnControl(UP_BUTTON, PUD_UP);
    pullUpDnControl(DOWN_BUTTON, PUD_UP);
    pullUpDnControl(LEFT_BUTTON, PUD_UP);
    pullUpDnControl(RIGHT_BUTTON, PUD_UP);

    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    std::thread countdownThread(showCountdown);
    countdownThread.join();
    std::thread timerThread(gameTimer);

    display.clear();
    drawMaze(maze, display, CELL_SIZE);
    drawPlayer();
    display.sendBuffer();

    while (true) {
        char ch = 0;
        if (digitalRead(UP_BUTTON) == LOW) ch = 'w';
        else if (digitalRead(DOWN_BUTTON) == LOW) ch = 's';
        else if (digitalRead(LEFT_BUTTON) == LOW) ch = 'a';
        else if (digitalRead(RIGHT_BUTTON) == LOW) ch = 'd';

        int newX = playerX;
        int newY = playerY;

        if (ch == 'w') {
            if (!maze.grid[playerY][playerX].walls[TOP]) newY--;
            else digitalWrite(BUZZER_PIN, HIGH);
        } else if (ch == 's') {
            if (!maze.grid[playerY][playerX].walls[BOTTOM]) newY++;
            else digitalWrite(BUZZER_PIN, HIGH);
        } else if (ch == 'a') {
            if (!maze.grid[playerY][playerX].walls[LEFT]) newX--;
            else digitalWrite(BUZZER_PIN, HIGH);
        } else if (ch == 'd') {
            if (!maze.grid[playerY][playerX].walls[RIGHT]) newX++;
            else digitalWrite(BUZZER_PIN, HIGH);
        } else {
            digitalWrite(BUZZER_PIN, LOW);
        }

        if (newX >= 0 && newX < MAZE_WIDTH && newY >= 0 && newY < MAZE_HEIGHT) {
            playerX = newX;
            playerY = newY;
        }

        display.clear();
        drawMaze(maze, display, CELL_SIZE);
        drawPlayer();
        display.sendBuffer();

        if (playerX == goalX && playerY == goalY) {
            gameRunning = false;
            timerThread.join();

            display.clear();
            drawYouWin(display);
            display.sendBuffer();

            while (digitalRead(UP_BUTTON) == HIGH &&
                   digitalRead(DOWN_BUTTON) == HIGH &&
                   digitalRead(LEFT_BUTTON) == HIGH &&
                   digitalRead(RIGHT_BUTTON) == HIGH) {
                usleep(50000);
            }

            execv("./maze_game", NULL); // restart game
        }

        usleep(100000);
    }

    return 0;
}
