#ifndef MAZE_H
#define MAZE_H

#include <vector>
#include <cstring>
#include <iostream>
#include <random>

#include <GLFW/glfw3.h>

using Edge = std::pair<int, int>;

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

class Maze {
private:
    int rows;
    int cols;
    int maze_rows;
    int maze_cols;
    int size;
    
    std::vector<Edge> vertical;
    std::vector<Edge> horizontal;
    std::vector<std::string> maze;

public:
    Maze(const int rows, const int cols, const std::vector<Edge>& MST);
    void generate();
    void print() const;
    void drawVerticalLine(float x, float y_from, float y_to) const;
    void drawHorizontalLine(float x_from, float x_to, float y) const;
    void draw() const;
    void display() const;
};

#endif // MAZE_H