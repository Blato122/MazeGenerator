#include "Maze.h"

Maze::Maze(const int rows, const int cols, const std::vector<Edge>& MST) : rows(rows), cols(cols), maze_rows(2 * rows - 1), maze_cols(2 * cols - 1), size(maze_rows * maze_cols) {
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
    if (!window) {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    //glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // black default?
    glClear(GL_COLOR_BUFFER_BIT);

    // !
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);

    glOrtho(-0.5, getWidth() + 0.5, getHeight() / 2 - 0.5, getHeight() + 0.5, -1, 1);
    glLineWidth(2.0f); // optional

    setBoundsStartAndFinish();

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        draw(); // chyba BARDZO niewydajne??
        drawBoundsStartAndFinish();
        drawRect(bottom_left_x, bottom_left_y, 1, 0, 0); // player
        glfwSwapBuffers(window);
        glfwPollEvents(); // co to robi i gdzie
    }

    glfwTerminate();
}

void Maze::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Maze* maze_ptr = static_cast<Maze*>(glfwGetWindowUserPointer(window));
    if (maze_ptr) {
        maze_ptr -> handleKeyPress(key, action);
    } else {
        std::cout << "keyCallback static_cast error" << std::endl;
        exit(1);
    }
}

void Maze::handleKeyPress(int key, int action) {
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        moveLeft();
    } else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        moveRight();
    } else if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        moveUp();
    } else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        moveDown();
    } else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        refresh();
    } else {
        return;
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