#include <iostream>
#include <chrono>

#include "Maze.h"
#include "Graph.h"

int main(int argc, char *argv[]) {
    int rows = 0;
    int cols = 0;

    if (argc == 3) {
        rows = atoi(argv[1]);
        cols = atoi(argv[2]);
    } else {
        std::cerr << "usage: ./a.out [rows] [columns]" << std::endl;
        return 1;
    }

    Graph g(rows, cols);
    g.kruskal();
    auto MST = g.getMST();

    bool keep_playing;
    do {
        Maze m(rows, cols, MST);
        m.generate();
        m.print();
        m.display();
        keep_playing = m.keepPlaying();
    } while (keep_playing);
}