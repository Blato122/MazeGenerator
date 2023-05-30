#ifndef MAZE_H
#define MAZE_H

#include <vector>
#include <cstring>
#include <iostream>
#include <random>
#include <cstdlib>

#include <GLFW/glfw3.h>

using Edge = std::pair<int, int>;

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

class Maze {
private:
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

public:
    Maze(const int rows, const int cols, const std::vector<Edge>& MST);
    void generate();
    void print() const;

    // funkcje od rysowania linii i rect wywalić do osobnej klasy może
    void drawVerticalLine(float x, float y_from, float y_to) const;
    void drawHorizontalLine(float x_from, float x_to, float y) const;
    void drawRect(float bl_x, float bl_y, float r, float g, float b) const;

    void draw() const;
    void setBoundsStartAndFinish();
    void drawBoundsStartAndFinish() const;
    void display();

    // width, height
    int getWidth() const;
    int getHeight() const;

    // the game part
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