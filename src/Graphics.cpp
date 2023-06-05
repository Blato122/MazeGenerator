#include "Graphics.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void Graphics::drawVerticalLine(float x, float y_from, float y_to) {
    glBegin(GL_LINES);
        glVertex2f(x, y_from);
        glVertex2f(x, y_to);
    glEnd();
}

void Graphics::drawHorizontalLine(float x_from, float x_to, float y) {
    glBegin(GL_LINES);
        glVertex2f(x_from, y);
        glVertex2f(x_to, y);
    glEnd();
}

void Graphics::drawVerticalLine(float x, float y_from, float y_to, float r, float g, float b) {
    if (r >= 0 && r <= 1 && g >= 0 && g <= 1 && b >= 0 && b <= 1) {
        glColor3f(r, g, b); // some color
    } else {
        glColor3f(1, 1, 1); // white
    }
    glBegin(GL_LINES);
        glVertex2f(x, y_from);
        glVertex2f(x, y_to);
    glEnd();
    glColor3f(1, 1, 1); // white again
}

void Graphics::drawHorizontalLine(float x_from, float x_to, float y, float r, float g, float b) {
    if (r >= 0 && r <= 1 && g >= 0 && g <= 1 && b >= 0 && b <= 1) {
        glColor3f(r, g, b); // some color
    } else {
        glColor3f(1, 1, 1); // white
    }
    glBegin(GL_LINES);
        glVertex2f(x_from, y);
        glVertex2f(x_to, y);
    glEnd();
    glColor3f(1, 1, 1); // white again
}

void Graphics::drawRect(float bl_x, float bl_y, float r, float g, float b) {
    glBegin(GL_QUADS);
        if (r >= 0 && r <= 1 && g >= 0 && g <= 1 && b >= 0 && b <= 1) {
            glColor3f(r, g, b); // some color
        } else {
            glColor3f(0, 0, 1); // blue
        }
        glVertex2f(bl_x + 0.5, bl_y + 0.25); // bottom-left
        glVertex2f(bl_x + 1.5, bl_y + 0.25); // bottom-right
        glVertex2f(bl_x + 1.5, bl_y + 0.75); // top-right
        glVertex2f(bl_x + 0.5, bl_y + 0.75); // top-left
        glColor3f(1, 1, 1); // white again
    glEnd();
}

void Graphics::displayImage(const int window_width, const int window_height, const int channels_,
std::string path, float min_x, float max_x, float min_y, float max_y) {
    int width = window_width;
    int height = window_height;
    int channels = channels_;

    stbi_uc* image = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!image) {
        std::cout << "Failed to load image " << path << std::endl;
        glfwTerminate();
        exit(1);
    }

    // Create texture ID
    GLuint textureID;
    glGenTextures(1, &textureID);

    // Bind texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image data into texture
    glTexImage2D(GL_TEXTURE_2D, 0, channels, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

    // Free image data
    stbi_image_free(image);

    // Enable 2D texturing
    glEnable(GL_TEXTURE_2D);
    // Start drawing the image
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(min_x, min_y); 

        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(max_x, min_y);

        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(max_x, max_y);

        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(min_x, max_y);
    glEnd();
}