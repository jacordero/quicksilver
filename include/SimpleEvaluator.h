//
// Created by Nikolay Yakovets on 2018-02-02.
//

#ifndef QS_SIMPLEEVALUATOR_H
#define QS_SIMPLEEVALUATOR_H


#include <memory>
#include <cmath>
#include "SimpleGraph.h"
#include "RPQTree.h"
#include "Evaluator.h"
#include "Graph.h"

class LRUCache {

    int maxSize = 40;
    std::deque<std::pair<std::string,std::shared_ptr<SimpleGraph>>> cache;

public:
    LRUCache();
    ~LRUCache() = default;
    std::shared_ptr<SimpleGraph> getFromCache(RPQTree *query);
    void addToCache(RPQTree *query, std::shared_ptr<SimpleGraph> &g);
};

class SimpleEvaluator : public Evaluator {

    LRUCache cache;
    std::shared_ptr<SimpleGraph> graph;
    std::shared_ptr<SimpleEstimator> est;

public:

    explicit SimpleEvaluator(std::shared_ptr<SimpleGraph> &g);
    ~SimpleEvaluator() = default;

    void prepare() override ;
    cardStat evaluate(RPQTree *query) override ;

    void attachEstimator(std::shared_ptr<SimpleEstimator> &e);

    std::shared_ptr<SimpleGraph> evaluate_aux(RPQTree *q, int depth);
    std::shared_ptr<SimpleGraph> evaluate_aux_preselected(RPQTree *q, std::set<int> preselectedVertices, int depth);
    static std::shared_ptr<SimpleGraph> project(uint32_t label, bool inverse, std::shared_ptr<SimpleGraph> &g);
    static std::shared_ptr<SimpleGraph> project_preselected(uint32_t label, bool inverse, std::shared_ptr<SimpleGraph> &g, std::set<int> preselectedVertices);
    static std::shared_ptr<SimpleGraph> join(std::shared_ptr<SimpleGraph> &left, std::shared_ptr<SimpleGraph> &right, int depth);

    static cardStat computeStats(std::shared_ptr<SimpleGraph> &g);
    static std::string treeToString(RPQTree *query);

};

#endif //QS_SIMPLEEVALUATOR_H
