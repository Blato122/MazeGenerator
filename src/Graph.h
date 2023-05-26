#ifndef GRAPH_H
#define GRAPH_H

#include <list>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <memory>

#include "DisjointSets.h"

using Edge = std::pair<int, int>;

class Graph {
private:
    int rows; // number of rows
    int cols; // number of columns
    int size; // number of vertices

    std::vector<Edge> G; // graph
    std::vector<Edge> MST; // minimal spanning tree
    // DisjointSets *ds;
    std::unique_ptr<DisjointSets> ds; // hehe

    void generate_edges();
    void add_edge(const int from, const int to);
    void kruskal();
    
public:
    Graph(const int rows, const int cols);
    // ~Graph();
    std::vector<Edge> getMST();
    static void print(const std::vector<Edge>& g);
};

#endif // GRAPH_H