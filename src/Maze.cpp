#include "Maze.h"

#define DEBUG

#ifdef DEBUG
#define Time(x) Timer(x)
#else
#define Time(x)
#endif // DEBUG

Maze::Maze(const int rows, const int cols, const std::vector<Edge>& MST) : rows(rows), cols(cols),
 maze_rows(2 * rows - 1), maze_cols(2 * cols - 1), size(maze_rows * maze_cols), you_win(false), 
 start_new_game(false), help_displayed(false), you_lose(false), pause_duration(0),
 MAX_TIME_SECS(std::sqrt(rows * cols)) {
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
                Graphics::drawHorizontalLine(col, col + 1.0, height - hrzn_row_fix);
            } else if (maze[row][col] == '|') {
                Graphics::drawVerticalLine(col, height - vert_row_fix, height - vert_row_fix - 1);
            } else if (maze[row][col] == ' ' && row % 2 == 1) { // only for horizontal rows
                int space_count = 1;
                int col_ = col;
                while (++col_ < width && maze[row][col_] == ' ') {
                    ++space_count;
                }

                if (space_count == 1 && col_ < width && maze[row][col_] != '|') { //!!
                    Graphics::drawHorizontalLine(col, col + 1.0, height - hrzn_row_fix);
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
    Graphics::drawHorizontalLine(0, upper_random_int, height);
    Graphics::drawHorizontalLine(upper_random_int + 2, width, height);
    Graphics::drawRect(upper_random_int, height - 1, 0, 1, 0); // finish rect

    Graphics::drawHorizontalLine(0, lower_random_int, height / 2);
    Graphics::drawHorizontalLine(lower_random_int + 2, width, height / 2);
    
    // generate vertical bounds
    Graphics::drawVerticalLine(0, height, height / 2);
    Graphics::drawVerticalLine(width, height, height / 2);

    glFlush();
}

void Maze::drawUpdateTimer() {
    glLineWidth(10.0f);
    Graphics::drawHorizontalLine(0, getWidth(), getHeight() + 1, 0, 0, 0); // cover previous timer
    const auto current_time = std::chrono::high_resolution_clock::now();
    const float time_elapsed = std::chrono::duration<float>((current_time - maze_start)).count() - pause_duration;
    const float time_left = MAX_TIME_SECS - time_elapsed;
    if (time_left <= 0) {
        timesUp();
        displayYouLose();
        glLineWidth(2.0f);
        return;
    }
    Graphics::drawHorizontalLine(0, getWidth() / (MAX_TIME_SECS / time_left), getHeight() + 1, 1 - (time_left / MAX_TIME_SECS), time_left / MAX_TIME_SECS, 0);
    glLineWidth(2.0f);
}

void Maze::timesUp() {
    you_lose = true;
}

void Maze::startTimer() {
    maze_start = std::chrono::high_resolution_clock::now();
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

    glOrtho(-0.5, getWidth() + 0.5, getHeight() / 2 - 0.5, getHeight() + 0.5 + 1, -1, 1);
    glLineWidth(2.0f); // optional

    setBoundsStartAndFinish();
    draw();
    drawBoundsStartAndFinish();
    startTimer();

    while (!start_new_game && !glfwWindowShouldClose(window)) {
        if (playerAtFinish()) {
            displayYouWin();
        }
        if ( !(you_win || help_displayed || you_lose) ) {
            Graphics::drawRect(bottom_left_x, bottom_left_y, 1, 0, 0); // player
            drawUpdateTimer(); // rysowanie ok ale ma być przerwane liczenie na ten czas!!
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
    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT) ) {
        if (you_win || help_displayed || you_lose) return;
        coverPreviousPlayerLocation();
        moveLeft();
    } else if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT) ) {
        if (you_win || help_displayed || you_lose) return;
        coverPreviousPlayerLocation();
        moveRight();
    } else if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT) ) {
        if (you_win || help_displayed || you_lose) return;
        coverPreviousPlayerLocation();
        moveUp();
    } else if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT) ) {
        if (you_win || help_displayed || you_lose) return;
        coverPreviousPlayerLocation();
        moveDown();
    } else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        if (you_win || help_displayed || you_lose) return;
        refresh();
    } else if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
        if (you_win || you_lose) {
            start_new_game = true;
        } 
    } else if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        if (! (you_win || you_lose) ) {
            help_displayed ? closeHelp() : displayHelp();
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

    const auto current_time = std::chrono::high_resolution_clock::now();
    pause_duration += std::chrono::duration<float>((current_time - pause_start)).count();
}

void Maze::displayHelp() {
    pause_start = std::chrono::high_resolution_clock::now();

    float min_x = -0.5f;
    float max_x = getWidth() + 0.5f;
    float min_y = getHeight() / 2 - 0.5f;
    float max_y = getHeight() + 0.5f;

    Graphics::displayImage(WINDOW_WIDTH, WINDOW_HEIGHT, 3, "images/help.jpg", min_x, max_x, min_y, max_y);

    help_displayed = true;
}

void Maze::displayYouLose() {
    float min_x = -0.5f;
    float max_x = getWidth() + 0.5f;
    float min_y = getHeight() / 2 - 0.5f;
    float max_y = getHeight() + 0.5f;

    Graphics::displayImage(WINDOW_WIDTH, WINDOW_HEIGHT, 3, "images/youlose.jpg", min_x, max_x, min_y, max_y);

    you_lose = true;
}

void Maze::displayYouWin() {
    float min_x = -0.5f;
    float max_x = getWidth() + 0.5f;
    float min_y = getHeight() / 2 - 0.5f;
    float max_y = getHeight() + 0.5f;

    Graphics::displayImage(WINDOW_WIDTH, WINDOW_HEIGHT, 3, "images/youlose.jpg", min_x, max_x, min_y, max_y);

    you_win = true;
}

void Maze::coverPreviousPlayerLocation() const {
    if (playerAtFinish()) {
        Graphics::drawRect(bottom_left_x, bottom_left_y, 0, 1, 0); // green trace so that finish doesn't get covered
    } else {
        Graphics::drawRect(bottom_left_x, bottom_left_y, 1, 1, 0); // yellow trace
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