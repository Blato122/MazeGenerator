#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <string>
#include <iostream>

#include <GLFW/glfw3.h>

class Graphics {
private:

public:
    static void drawVerticalLine(float x, float y_from, float y_to); // white (default?)
    static void drawHorizontalLine(float x_from, float x_to, float y); // white (default?)
    static void drawVerticalLine(float x, float y_from, float y_to, float r, float g, float b);
    static void drawHorizontalLine(float x_from, float x_to, float y, float r, float g, float b);
    static void drawRect(float bl_x, float bl_y, float r, float g, float b);
    static void displayImage(const int window_width, const int window_height, const int channels_, 
    std::string path, float min_x, float max_x, float min_y, float max_y);
};

#endif // GRAPHICS_H