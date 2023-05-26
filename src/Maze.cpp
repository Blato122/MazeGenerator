#include "Maze.h"

Maze::Maze(const int rows, const int cols, const std::vector<Edge>& MST) : rows(rows), cols(cols), maze_rows(2 * rows - 1), maze_cols(2 * cols - 1), size(maze_rows * maze_cols) {
    for (const Edge& e : MST) {
        if (e.first + 1 == e.second) {
            horizontal.push_back(e);
        } else if (e.first + cols == e.second) {
            vertical.push_back(e);
        }
    }

    std::sort(vertical.begin(), vertical.end(), [](Edge& e1, Edge& e2) { //!!
        return static_cast<bool>(e1.first < e2.first);
    });
    std::sort(horizontal.begin(), horizontal.end(), [](Edge& e1, Edge& e2) {
        return static_cast<bool>(e1.first < e2.first);
    });
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
    for (const std::string& row : maze) {
        std::cout << row << "\n";
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

void Maze::draw() const {
    int width = maze[0].length();
    int height = maze.size();
    std::cout << width << " " << height << std::endl;

    glOrtho(-0.5, width + 0.5, height / 2 - 0.5, height + 0.5, -1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glLineWidth(2.0f); // optional - allegedly not supported by some... things (?)

    int vert_row_fix = 0;
    int hrzn_row_fix = 0;

    // random hole
    std::random_device seed;
    std::mt19937 gen(seed());
    std::uniform_int_distribution<int> uni(1, (width - 1) / 2);

    // generate top and bottom bounds with one hole (start and finish)
    int random_int = uni(gen) * 2; // make it even
    std::cout << (width - 1) / 2 * 2 << " " << random_int << std::endl;
    drawHorizontalLine(0, random_int, height);
    drawHorizontalLine(random_int + 2, width, height);

    random_int = uni(gen) * 2; // make it even
    std::cout << (width - 1) / 2 * 2 << " " << random_int << std::endl;
    drawHorizontalLine(0, random_int, height / 2);
    drawHorizontalLine(random_int + 2, width, height / 2);
    
    // generate vertical bounds
    drawVerticalLine(0, height, height / 2);
    drawVerticalLine(width, height, height / 2);

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

void Maze::display() const {
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
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    draw();
    glfwSwapBuffers(window); // outside the loop because maze doesn't require continuous screen updates

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwTerminate();

}
