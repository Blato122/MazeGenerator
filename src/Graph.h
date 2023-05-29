#ifndef GRAPH_H
#define GRAPH_H

#include <list>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <memory>
#include <cstdlib>

#include "DisjointSets.h"

using Edge = std::pair<int, int>;

class Graph {
private:
    int rows; // number of rows
    int cols; // number of columns
    int size; // number of vertices

    std::vector<Edge> G; // graph
    std::vector<Edge> MST; // minimal spanning tree
    std::unique_ptr<DisjointSets> ds;

    void generate_edges();
    void add_edge(const int from, const int to);
    void kruskal();
    
public:
    Graph(const int rows, const int cols);
    std::vector<Edge> getMST();
    static void print(const std::vector<Edge>& g);
};

#endif // GRAPH_H