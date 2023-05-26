#ifndef DISJOINT_SETS_H
#define DISJOINT_SETS_H

#include <vector>

#define null -1

class DisjointSets {
private:
    class Node {
    public: // ok?
        int data;
        int parent;
        int rank;
        Node(const int data_);
    };
    std::vector<Node> tab;

public:
    DisjointSets(const int size);
    void join(const int x, const int y);
    int find(const int x);
};

#endif // DISJOINT_SETS_H