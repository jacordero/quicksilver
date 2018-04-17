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

class SimpleIndex {
    std::map<std::string, std::shared_ptr<SimpleGraph>> index;

public:
    SimpleIndex();
    ~SimpleIndex() = default;
    std::shared_ptr<SimpleGraph> getFromIndex(std::string key);
    void addToIndex(std::string key, std::shared_ptr<SimpleGraph> &g);
};


class SimpleEvaluator : public Evaluator {

    LRUCache cache;
    std::shared_ptr<SimpleGraph> graph;
    std::shared_ptr<SimpleEstimator> est;
    SimpleIndex index;
    bool enableCache;
    bool smartEnabled;
    int plannerType;

public:

    explicit SimpleEvaluator(std::shared_ptr<SimpleGraph> &g);
    ~SimpleEvaluator() = default;

    void prepare() override ;
    cardStat evaluate(RPQTree *query) override ;

    void attachEstimator(std::shared_ptr<SimpleEstimator> &e);

    std::shared_ptr<SimpleGraph> evaluate_aux(RPQTree *q, int depth);
    std::shared_ptr<SimpleGraph> evaluate_aux_preselected(RPQTree *q, std::set<int> preselectedVertices, int depth);
    std::shared_ptr<SimpleGraph> evaluate_preselected_for_left_tree(RPQTree *q, std::set<int> preselectedStartVertices, int depth);
    std::shared_ptr<SimpleGraph> evaluate_preselected_for_right_tree(RPQTree *q, std::set<int> preselectedEndVertices, int depth);


    static std::shared_ptr<SimpleGraph> project(uint32_t label, bool inverse, std::shared_ptr<SimpleGraph> &g);
    static std::shared_ptr<SimpleGraph> project_preselected(uint32_t label, bool inverse, std::shared_ptr<SimpleGraph> &g, std::set<int> preselectedVertices);
    static std::shared_ptr<SimpleGraph> project_preselected_for_left_tree(uint32_t label, bool inverse, std::shared_ptr<SimpleGraph> &g, std::set<int> preselectedStartVertices);
    static std::shared_ptr<SimpleGraph> project_preselected_for_right_tree(uint32_t label, bool inverse, std::shared_ptr<SimpleGraph> &g, std::set<int> preselectedEndVertices);


    static std::shared_ptr<SimpleGraph> join(std::shared_ptr<SimpleGraph> &left, std::shared_ptr<SimpleGraph> &right, int depth);
    static std::shared_ptr<SimpleGraph> smart_join(std::shared_ptr<SimpleGraph> &left, std::shared_ptr<SimpleGraph> &right, int depth);

    static cardStat computeStats(std::shared_ptr<SimpleGraph> &g);
    static std::string treeToString(RPQTree *query);
    void createIndex(int numberLabels);

    static std::string treeToStringForTokens(RPQTree *query);
    std::vector<std::string> extractTokens(std::string queryStr);
    int findPivotIndex(std::vector<std::string> tokens);

};

#endif //QS_SIMPLEEVALUATOR_H
