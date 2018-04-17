//
// Created by Nikolay Yakovets on 2018-01-31.
//

#ifndef QS_SIMPLEGRAPH_H
#define QS_SIMPLEGRAPH_H

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <set>
#include <iostream>
#include <regex>
#include <fstream>
#include "Graph.h"


class SimpleGraph : public Graph {
public:

    std::map<uint32_t, std::vector<std::pair<uint32_t, uint32_t>>> adj;
    std::map<uint32_t, std::vector<std::pair<uint32_t, uint32_t>>> reverse_adj;
    std::set<int> startVertices;
    std::set<int> endVertices;
    //std::vector<std::vector<std::pair<uint32_t,uint32_t>>> adj;
    //std::vector<std::vector<std::pair<uint32_t,uint32_t>>> reverse_adj; // vertex adjacency list
protected:
    uint32_t V;
    uint32_t L;

public:

    SimpleGraph() : V(0), L(0) {};
    ~SimpleGraph() = default;
    explicit SimpleGraph(uint32_t n);

    uint32_t getNoVertices() const override ;
    uint32_t getNoEdges() const override ;
    uint32_t getNoDistinctEdges() const override ;
    uint32_t getUniqueEdgesForStat() const;
    uint32_t getNoLabels() const override ;

    bool sortEdges(const std::pair<uint32_t,uint32_t> &a, const std::pair<uint32_t,uint32_t> &b);
    void addEdge(uint32_t from, uint32_t to, uint32_t edgeLabel) override ;
    void readFromContiguousFile(const std::string &fileName) override ;

    void setNoVertices(uint32_t n);
    void setNoLabels(uint32_t noLabels);

    void setEndVertices(std::set<int> newEndVertices);
    void setStartVertices(std::set<int> newStartVertices);
    int computeEndVertices();
    std::set<int> getEndVertices();

    void printStartVertices();
    void printEndVertices();
    void printGraph();

};

#endif //QS_SIMPLEGRAPH_H
