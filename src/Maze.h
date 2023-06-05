#ifndef MAZE_H
#define MAZE_H

#include <vector>
#include <cstring>
#include <iostream>
#include <random>
#include <cstdlib>
#include <chrono>
#include <cmath>

#include "Timer.h"
#include "Graphics.h"

#include <GLFW/glfw3.h>

using Edge = std::pair<int, int>;

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

class Maze {
private:
    const float MAX_TIME_SECS;
    // strange variables (but I do use them in some functions)
    // in order to get width or height, use getWidth() and getHeight()
    int rows;
    int cols;
    int maze_rows; // nie jest to czasami to samo co width i height
    int maze_cols; // ?
    int size;

    // current position
    float bottom_left_x;
    float bottom_left_y;

    int upper_random_int; // upper bound - inne nazwy chyba lepsze...
    int lower_random_int; // lower bound
    
    std::vector<Edge> vertical;
    std::vector<Edge> horizontal;
    std::vector<std::string> maze;

    bool you_win;
    bool start_new_game;
    bool help_displayed;
    bool you_lose;

    std::chrono::time_point<std::chrono::high_resolution_clock> maze_start;
    std::chrono::time_point<std::chrono::high_resolution_clock> pause_start;
    float pause_duration; // total duration

    GLFWwindow* window_global; // huh

public:
    Maze(const int rows, const int cols, const std::vector<Edge>& MST);
    void generate();
    void print() const;

    void draw() const;
    void setBoundsStartAndFinish();
    void drawBoundsStartAndFinish() const;
    void drawUpdateTimer();
    void display();

    // width, height
    int getWidth() const;
    int getHeight() const;

    // the game part
    void timesUp();
    void startTimer();
    bool playerAtFinish() const;
    void closeHelp();
    void displayHelp();
    void displayYouWin();
    void displayYouLose();
    bool keepPlaying() const;
    void coverPreviousPlayerLocation() const;
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    void handleKeyPress(int key, int action);
    void refresh();
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

#endif // MAZE_H