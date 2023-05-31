#include "Maze.h"
#include "Timer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define DEBUG

#ifdef DEBUG
#define Time(x) Timer(x)
#else
#define Time(x)
#endif // DEBUG

Maze::Maze(const int rows, const int cols, const std::vector<Edge>& MST) : rows(rows), cols(cols),
 maze_rows(2 * rows - 1), maze_cols(2 * cols - 1), size(maze_rows * maze_cols), you_win(false), 
 start_new_game(false), help_displayed(false) {
    for (const Edge& e : MST) {
        if (e.first + 1 == e.second) {
            horizontal.push_back(e);
        } else if (e.first + cols == e.second) {
            vertical.push_back(e);
        }
    }

    std::sort(vertical.begin(), vertical.end(), [](Edge& e1, Edge& e2) { // !!!
        return static_cast<bool>(e1.first < e2.first);
    });
    std::sort(horizontal.begin(), horizontal.end(), [](Edge& e1, Edge& e2) {
        return static_cast<bool>(e1.first < e2.first);
    });
}

int Maze::getWidth() const {
    return maze[0].length();
}

int Maze::getHeight() const{
    return maze.size();
}

void Maze::generate() {
    auto t = Time("generate()");

    int horizontal_row_count = 0;
    int vertical_row_count = 0;
    std::vector<Edge> vertical_(vertical);
    std::vector<Edge> horizontal_(horizontal);

    for (int i = 0; i < maze_rows; ++i) {

        std::string maze_row;
        maze_row += " "; // !!!

        if (i % 2 == 0) { // even rows (starting with 0)
            for (int j = 0; j < (maze_cols - 1) / 2; ++j) {
                auto it = std::find_if(horizontal_.begin(), horizontal_.end(), [&](Edge& e) {
                    return e.first < (j + 1) + cols * horizontal_row_count;
                });
                if (it != horizontal_.end()) {
                    maze_row += "  ";
                    horizontal_.erase(it);
                } else {
                    maze_row += " |";
                }
            }
            maze_row += " ";
            ++horizontal_row_count;
        } 
        
        else { // odd rows
            for (int j = 0; j < ((maze_cols - 1) / 2) + 1; ++j) {
                bool last = (j + 1) >= ((maze_cols - 1) / 2) + 1;
                auto it = std::find_if(vertical_.begin(), vertical_.end(), [&](Edge& e) {
                    return e.first < (j + 1) + cols * vertical_row_count;
                });
                if (it != vertical_.end()) {
                    maze_row += (last ? " " : "  ");
                    vertical_.erase(it);
                } else {
                    maze_row += (last ? "-" : "- ");
                }
            }
            ++vertical_row_count;
        }

        maze.push_back(maze_row);
    }
}

void Maze::print() const {
    int i = 0;
    for (const std::string& row : maze) {
        std::cout << i++ << ": " << row << "\n";
    }
}

void Maze::drawVerticalLine(float x, float y_from, float y_to) const {
    glBegin(GL_LINES);
        glVertex2f(x, y_from);
        glVertex2f(x, y_to);
    glEnd();
}

void Maze::drawHorizontalLine(float x_from, float x_to, float y) const {
    glBegin(GL_LINES);
        glVertex2f(x_from, y);
        glVertex2f(x_to, y);
    glEnd();
}

void Maze::drawRect(float bl_x, float bl_y, float r, float g, float b) const {
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

void Maze::draw() const {
    int width = getWidth();
    int height = getHeight();

    int vert_row_fix = 0;
    int hrzn_row_fix = 0;

    // draw actual maze
    for (int row = 0; row < height; ++row) {
        if (row % 2 == 1) {
            ++hrzn_row_fix;
        }

        if (row % 2 == 0 && row != 0) {
            ++vert_row_fix;
        } 

        for (int col = 0; col < width; ++col) {
            if (maze[row][col] == '-') {
                drawHorizontalLine(col, col + 1.0, height - hrzn_row_fix);
            } else if (maze[row][col] == '|') {
                drawVerticalLine(col, height - vert_row_fix, height - vert_row_fix - 1);
            } else if (maze[row][col] == ' ' && row % 2 == 1) { // only for horizontal rows
                int space_count = 1;
                int col_ = col;
                while (++col_ < width && maze[row][col_] == ' ') {
                    ++space_count;
                }

                if (space_count == 1 && col_ < width && maze[row][col_] != '|') { //!!
                    drawHorizontalLine(col, col + 1.0, height - hrzn_row_fix);
                }
            }
        }
    }

    glFlush();
}

void Maze::setBoundsStartAndFinish() {
    int width = getWidth();
    int height = getHeight();

    // random hole
    std::random_device seed;
    std::mt19937 gen(seed());
    std::uniform_int_distribution<int> uni(1, (width - 1) / 2);

    upper_random_int = uni(gen) * 2; // make it even
    lower_random_int = uni(gen) * 2; // make it even

    // draw player's object using that later
    bottom_left_x = lower_random_int;
    bottom_left_y = height / 2;
}

void Maze::drawBoundsStartAndFinish() const {
    int width = getWidth();
    int height = getHeight();

    // generate top and bottom bounds with one hole (start and finish)
    drawHorizontalLine(0, upper_random_int, height);
    drawHorizontalLine(upper_random_int + 2, width, height);
    drawRect(upper_random_int, height - 1, 0, 1, 0); // finish rect

    drawHorizontalLine(0, lower_random_int, height / 2);
    drawHorizontalLine(lower_random_int + 2, width, height / 2);
    
    // generate vertical bounds
    drawVerticalLine(0, height, height / 2);
    drawVerticalLine(width, height, height / 2);

    glFlush();
}

void Maze::display() {
    // initialize GLFW
    if (!glfwInit()) {
        return;
    }

    // create window
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Maze", NULL, NULL);
    window_global = window;
    if (!window) {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);

    glOrtho(-0.5, getWidth() + 0.5, getHeight() / 2 - 0.5, getHeight() + 0.5, -1, 1);
    glLineWidth(2.0f); // optional

    setBoundsStartAndFinish();
    draw();
    drawBoundsStartAndFinish();

    while (!start_new_game && !glfwWindowShouldClose(window)) {
        if ( !(you_win || help_displayed) ) {
            drawRect(bottom_left_x, bottom_left_y, 1, 0, 0); // player
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}

void Maze::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Maze* maze_ptr = static_cast<Maze*>(glfwGetWindowUserPointer(window));
    if (maze_ptr) {
        maze_ptr -> handleKeyPress(key, action);
    } else {
        std::cout << "keyCallback static_cast error" << std::endl;
        glfwTerminate();
        exit(1);
    }
}

void Maze::handleKeyPress(int key, int action) {
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        if (you_win) return;
        coverPreviousPlayerLocation();
        moveLeft();
    } else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        if (you_win) return;
        coverPreviousPlayerLocation();
        moveRight();
    } else if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        if (you_win) return;
        coverPreviousPlayerLocation();
        moveUp();
    } else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        if (you_win) return;
        coverPreviousPlayerLocation();
        moveDown();
    } else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        if (you_win) return;
        refresh();
    } else if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
        if (you_win) {
            start_new_game = true;
        } else if (playerAtFinish()) {
            displayYouWin();
        }
    } else if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        if (!you_win) {
            if (help_displayed) {
                closeHelp();
            } else {
                displayHelp();
            }
        }
    } else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwTerminate();
        exit(1);
    } else {
        return;
    }

    std::cout << bottom_left_x << " " << bottom_left_y << std::endl;
}

bool Maze::keepPlaying() const {
    return start_new_game;
}

bool Maze::playerAtFinish() const {
    return (bottom_left_x == upper_random_int && bottom_left_y == getHeight() - 1);
}

void Maze::closeHelp() {
    glClear(GL_COLOR_BUFFER_BIT);
    draw();
    drawBoundsStartAndFinish();
    help_displayed = false;
}

void Maze::displayHelp() {
    int width = WINDOW_WIDTH;
    int height = WINDOW_HEIGHT;
    int channels = 3;

    stbi_uc* image = stbi_load("images/help.jpg", &width, &height, &channels, 0);
    if (!image) {
        std::cout << "Failed to load image" << std::endl;
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
        glVertex2f(-0.5f, getHeight() / 2 - 0.5f);

        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(getWidth() + 0.5f, getHeight() / 2 - 0.5f);

        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(getWidth() + 0.5f, getHeight() + 0.5f);

        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-0.5f, getHeight() + 0.5f);
    glEnd();

    help_displayed = true;
}

void Maze::displayYouWin() {
    int width = WINDOW_WIDTH;
    int height = WINDOW_HEIGHT;
    int channels = 3;

    stbi_uc* image = stbi_load("images/youwin.jpg", &width, &height, &channels, 0);
    if (!image) {
        std::cout << "Failed to load image" << std::endl;
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
        glVertex2f(-0.5f, getHeight() / 2 - 0.5f);

        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(getWidth() + 0.5f, getHeight() / 2 - 0.5f);

        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(getWidth() + 0.5f, getHeight() + 0.5f);

        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-0.5f, getHeight() + 0.5f);
    glEnd();

    you_win = true;
}

void Maze::coverPreviousPlayerLocation() const {
    if (playerAtFinish()) {
        drawRect(bottom_left_x, bottom_left_y, 0, 1, 0); // green trace so that finish doesn't get covered
    } else {
        drawRect(bottom_left_x, bottom_left_y, 1, 1, 0); // yellow trace
    }
}

void Maze::refresh() {
    bottom_left_x = lower_random_int;
    bottom_left_y = getHeight() / 2;
    glClear(GL_COLOR_BUFFER_BIT);
    draw();
    drawBoundsStartAndFinish();
}

void Maze::moveLeft() {
    if (bottom_left_x < 2) { // out of bounds
        return;
    }

    if (maze[bottom_left_y * 2 - 2 * (bottom_left_y * 2 - (getHeight() - 1))][bottom_left_x] == '|') {
        return;
    }

    bottom_left_x -= 2;
}

void Maze::moveRight() {
    if (bottom_left_x + 1 >= getWidth() - 2) { // out of bounds
        return;
    }

    // why +2?
    if (maze[bottom_left_y * 2 - 2 * (bottom_left_y * 2 - (getHeight() - 1))][bottom_left_x + 2] == '|') {
        return;
    }

    bottom_left_x += 2;
}

void Maze::moveUp() {
    if (bottom_left_y + 1 >= getHeight()) { // out of bounds
        return;
    }

    // trochę dziwne ale działa...
    if (maze[bottom_left_y * 2 - 2 * (bottom_left_y * 2 - (getHeight() - 1)) - 1][bottom_left_x + 1] == '-') {
        return;
    }

    bottom_left_y += 1;
}

void Maze::moveDown() {
    if (bottom_left_y < 1 + getHeight() / 2) { // out of bounds
        return;
    }

    // trochę dziwne ale działa...
    if (maze[bottom_left_y * 2 - 2 * (bottom_left_y * 2 - (getHeight() - 1)) + 1][bottom_left_x + 1] == '-') {
        return;
    }

    bottom_left_y -= 1;
}