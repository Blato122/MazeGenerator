#include "Graph.h"
#include "Timer.h"

#define DEBUG

#ifdef DEBUG
#define Time(x) Timer(x)
#else
#define Time(x)
#endif // DEBUG

Graph::Graph(const int rows_, const int cols_) : rows(rows_), cols(cols_), size(rows * cols), ds(std::make_unique<DisjointSets>(size)) {
    if (rows < 2 || cols < 2) {
        exit(1);
    }
    generate_edges();
}

void Graph::generate_edges() {
    for (int i = 0; i + cols < size; ++i) {
        add_edge(i, i + cols); // vertical
    }

    // "size - 1" to ostatni node
    for (int i = 0; i + 1 < size; ++i) { 
        if ((i + 1) % cols != 0) {
            add_edge(i, i + 1); // horizontal
        }
    }
}

void Graph::add_edge(const int from, const int to) {
    G.push_back(std::make_pair(from, to));
}

void Graph::kruskal() {
    auto t = Time("kruskal()");

    int u, v;

    std::random_device seed;
    std::mt19937 gen(seed());
    std::shuffle(G.begin(), G.end(), gen);

    for (const Edge& e : G) {
        u = ds -> find(e.first);
        v = ds -> find(e.second);
        if (u != v) {
            MST.push_back(e);
            ds -> join(u, v);
        }
    }
}

std::vector<Edge> Graph::getMST() const {
    return MST;
}

void Graph::print(const std::vector<Edge>& g) {
    for (const Edge& e : g) {
        std::cout << e.first << " " << e.second << std::endl;
    }
}