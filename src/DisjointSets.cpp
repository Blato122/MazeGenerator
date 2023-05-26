#include "DisjointSets.h"

DisjointSets::Node::Node(const int data_) : data(data_), parent(null), rank(0) {}

DisjointSets::DisjointSets(const int size) {
    for (int i = 0; i < size; ++i) {
        tab.push_back(Node(i));
    }
}

void DisjointSets::join(const int x, const int y) {
    int x_root = find(x);
    int y_root = find(y);

    if (tab[x_root].rank > tab[y_root].rank) {
        tab[y_root].parent = x_root;
    } else if (tab[x_root].rank < tab[y_root].rank) {
        tab[x_root].parent = y_root;
    } else if (x_root != y_root) {
        tab[y_root].parent = x_root;
        tab[x_root].rank += 1;
    }
}

int DisjointSets::find(const int x) {
    if (tab[x].parent == null) {
        return x;
    }

    tab[x].parent = find(tab[x].parent);
    return tab[x].parent;
}