//
// Created by Nikolay Yakovets on 2018-02-02.
//

#include "SimpleEstimator.h"
#include "SimpleEvaluator.h"

SimpleEvaluator::SimpleEvaluator(std::shared_ptr<SimpleGraph> &g) {

    // works only with SimpleGraph
    graph = g;
    est = nullptr; // estimator not attached by default
    cache;
}

void SimpleEvaluator::attachEstimator(std::shared_ptr<SimpleEstimator> &e) {
    est = e;
}

void SimpleEvaluator::prepare() {

    // if attached, prepare the estimator
    if(est != nullptr) {
        /** Estimator is being prepared**/
        //std::cout << "Estimator is being prepared\n";
        //est->prepare();
    }

    // prepare other things here.., if necessary

}

cardStat SimpleEvaluator::computeStats(std::shared_ptr<SimpleGraph> &g) {

    cardStat stats {};

    // TODO: modify this code to use the map iterator
    for (auto &mapEntry : g->adj) {
        stats.noOut += mapEntry.second.size();
    }

    stats.noPaths = g->getNoDistinctEdges();

    // TODO: modify this code to use the map iterator
    for (auto &mapEntry : g->reverse_adj) {
        stats.noIn += mapEntry.second.size();
    }

    return stats;
}

std::shared_ptr<SimpleGraph> SimpleEvaluator::project(uint32_t projectLabel, bool inverse, std::shared_ptr<SimpleGraph> &in) {

    auto out = std::make_shared<SimpleGraph>(in->getNoVertices());
    out->setNoLabels(in->getNoLabels());

    if(!inverse) {
        // going forward
        for (auto &mapEntry : in->adj) {

            uint32_t source = mapEntry.first;
            auto sourceVec = mapEntry.second;
            for (auto labelTarget : sourceVec) {

                auto label = labelTarget.first;
                auto target = labelTarget.second;

                if (label == projectLabel){
                    out->addEdge(source, target, label);
                }

            }
        }
    } else {
        // going backward
        // TODO: implement preselected vertices for backward mode
        for (auto &mapEntry : in->reverse_adj) {

            uint32_t source = mapEntry.first;
            auto sourceVec = mapEntry.second;
            for (auto labelTarget : sourceVec) {

                auto label = labelTarget.first;
                auto target = labelTarget.second;

                if (label == projectLabel){
                    out->addEdge(source, target, label);
                }
            }
        }
    }

    return out;
}

std::shared_ptr<SimpleGraph> SimpleEvaluator::project_preselected(uint32_t projectLabel, bool inverse, std::shared_ptr<SimpleGraph> &in, std::set<int> preselectedVertices) {

    auto out = std::make_shared<SimpleGraph>(in->getNoVertices());
    out->setNoLabels(in->getNoLabels());

    //std::cout << "Preselected vertices size: " << preselectedVertices.size() << std::endl;
    std::set<int> newEndVertices;
    std::set<int> newStartVertices;

    if(!inverse) {
        // going forward
        //for (auto &mapEntry : in->adj) {
        for (int source: preselectedVertices){

            //uint32_t source = mapEntry.first;
            //auto sourceVec = mapEntry.second;
            auto sourceVec = in->adj[source];
            for (auto labelTarget : sourceVec) {

                auto label = labelTarget.first;
                auto target = labelTarget.second;

                if (label == projectLabel){
                    out->addEdge(source, target, label);
                    newEndVertices.insert(target);
                    newStartVertices.insert(source);
                }

            }
        }
    } else {
        // going backward
        // TODO: implement preselected vertices for backward mode
        for (auto &mapEntry : in->reverse_adj) {

            uint32_t source = mapEntry.first;
            auto sourceVec = mapEntry.second;
            for (auto labelTarget : sourceVec) {

                auto label = labelTarget.first;
                auto target = labelTarget.second;

                if (label == projectLabel){
                    out->addEdge(source, target, label);
                    newEndVertices.insert(target);
                    newStartVertices.insert(source);
                }
            }
        }
    }

    out->setStartVertices(newStartVertices);
    out->setEndVertices(newEndVertices);

    return out;
}



std::shared_ptr<SimpleGraph> SimpleEvaluator::join(std::shared_ptr<SimpleGraph> &left, std::shared_ptr<SimpleGraph> &right, int depth) {

    auto out = std::make_shared<SimpleGraph>(left->getNoVertices());
    out->setNoLabels(1);

    //for(uint32_t leftSource = 0; leftSource < left->getNoVertices(); leftSource++) {
    //std::cout << std::string(depth, '\t') << "[x] join left size: " << left->adj.size() << std::endl;
    //std::cout << std::string(depth, '\t') << "[x] join right size: " << right->adj.size() << std::endl;
    std::set<int> newEndVertices;
    std::set<int> newStartVertices;

    for(auto &mapEntry: left->adj){

        uint32_t leftSource = mapEntry.first;
        auto leftSourceVec = mapEntry.second;
        for (auto labelTarget : leftSourceVec) {

            int leftTarget = labelTarget.second;
            // try to join the left target with right source
            for (auto rightLabelTarget : right->adj[leftTarget]) {

                auto rightTarget = rightLabelTarget.second;
                out->addEdge(leftSource, rightTarget, 0);
                newEndVertices.insert(rightTarget);
                newStartVertices.insert(leftSource);
            }
        }
    }

    out->setStartVertices(newStartVertices);
    out->setEndVertices(newEndVertices);

    //std::cout << std::string(depth, '\t') << "[x] join output size: " << out->adj.size() << std::endl;

    return out;
}

std::shared_ptr<SimpleGraph> SimpleEvaluator::evaluate_aux(RPQTree *q, int depth) {

    // evaluate according to the AST bottom-up

    std::shared_ptr<SimpleGraph> g = cache.getFromCache(q);
    if (g != nullptr){
        //std::cout << "Cache hit: " << treeToString(q) <<"\n";
        return g;
    }

    if(q->isLeaf()) {
        //std::cout << std::string(depth, '\t') <<  "Is leaf: " << q->data << std::endl;

        // project out the label in the AST
        std::regex directLabel (R"((\d+)\+)");
        std::regex inverseLabel (R"((\d+)\-)");

        std::smatch matches;

        uint32_t label;
        bool inverse;

        if(std::regex_search(q->data, matches, directLabel)) {
            label = (uint32_t) std::stoul(matches[1]);
            inverse = false;
        } else if(std::regex_search(q->data, matches, inverseLabel)) {
            label = (uint32_t) std::stoul(matches[1]);
            inverse = true;
        } else {
            std::cerr << "Label parsing failed!" << std::endl;
            return nullptr;
        }

        g = SimpleEvaluator::project(label, inverse, graph);
        cache.addToCache(q, g);
        return g;
    }

    if(q->isConcat()) {



        //std::cout << std::string(depth, '\t') << "Is concat: " << q->data << std::endl;

        // evaluate the children
        //std::cout << std::string(depth, '\t') << "left: ";
        //(q->left)->print();
        //std::cout << "\n";
        //std::cout << std::string(depth, '\t') << "right: ";
        //(q->right)->print();
        //std::cout << std::string(depth, '\t') << "\n";


        auto leftGraph = SimpleEvaluator::evaluate_aux(q->left, depth + 1);
        auto rightGraph = SimpleEvaluator::evaluate_aux(q->right, depth + 1);

        //std::cout << std::string(depth, '\t') << "Graph vertices: " << graph->getNoVertices() << std::endl;
        //std::cout << std::string(depth, '\t') << "Left graph vertices: " << leftGraph->adj.size() << std::endl;
        //std::cout << std::string(depth, '\t') << "Left end computed vertices: " << leftGraph->computeEndVertices() << std::endl;
        //std::cout << std::string(depth, '\t') << "Right graph vertices: " << rightGraph->adj.size() << std::endl;


        // join left with right
        g = SimpleEvaluator::join(leftGraph, rightGraph, depth);
        //g->printGraph();
        //g->printStartVertices();
        //g->printEndVertices();
        cache.addToCache(q, g);
        return g;
    }

    return nullptr;
}

std::shared_ptr<SimpleGraph> SimpleEvaluator::evaluate_aux_preselected(RPQTree *q, std::set<int> preselectedVertices, int depth) {

    // evaluate according to the AST bottom-up
    std::shared_ptr<SimpleGraph> g = cache.getFromCache(q);
    if (g != nullptr){
        //std::cout << "Cache hit: " << treeToString(q) <<"\n";
        return g;
    }

    if(q->isLeaf()) {
        //std::cout  << std::string(depth, '\t') << "Is leaf: " << q->data << std::endl;

        // project out the label in the AST
        std::regex directLabel (R"((\d+)\+)");
        std::regex inverseLabel (R"((\d+)\-)");

        std::smatch matches;

        uint32_t label;
        bool inverse;

        if(std::regex_search(q->data, matches, directLabel)) {
            label = (uint32_t) std::stoul(matches[1]);
            inverse = false;
        } else if(std::regex_search(q->data, matches, inverseLabel)) {
            label = (uint32_t) std::stoul(matches[1]);
            inverse = true;
        } else {
            std::cerr << "Label parsing failed!" << std::endl;
            return nullptr;
        }

        g = SimpleEvaluator::project_preselected(label, inverse, graph, preselectedVertices);
        //cache.addToCache(q, g);
        return g;
    }

    if(q->isConcat()) {



        //std::cout << std::string(depth, '\t') << "Is concat: " << q->data << std::endl;

        // evaluate the children
        //std::cout << std::string(depth, '\t') << "left: ";
        //(q->left)->print();
        //std::cout << "\n";
        //std::cout << std::string(depth, '\t') << "right: ";
        //(q->right)->print();
        //std::cout << std::string(depth, '\t') << "\n";


        auto leftGraph = SimpleEvaluator::evaluate_aux_preselected(q->left, preselectedVertices, depth+1);
        auto rightGraph = SimpleEvaluator::evaluate_aux_preselected(q->right, leftGraph->endVertices, depth + 1);

        //std::cout  << std::string(depth, '\t') << "Graph vertices: " << graph->getNoVertices() << std::endl;
        //std::cout  << std::string(depth, '\t') << "Left graph vertices: " << leftGraph->adj.size() << std::endl;
        //std::cout  << std::string(depth, '\t') << "Left graph end vertices: " << leftGraph->getEndVertices().size() << std::endl;
        //std::cout  << std::string(depth, '\t') << "Right graph vertices: " << rightGraph->adj.size() << std::endl;


        // join left with right
        g = SimpleEvaluator::join(leftGraph, rightGraph, depth);
        //g->printGraph();
        //g->printStartVertices();
        //g->printEndVertices();
        cache.addToCache(q, g);
        return g;
    }

    return nullptr;
}


cardStat SimpleEvaluator::evaluate(RPQTree *query) {
    std::cout <<"\n";
    //auto res = evaluate_aux(query, 0);
    auto res = evaluate_aux_preselected(query, graph->endVertices, 0);
    return SimpleEvaluator::computeStats(res);
}


std::string SimpleEvaluator::treeToString(RPQTree *query){
    std::string str;
    if (query->left == nullptr && query->right == nullptr){
        str.append(query->data);
    } else {
        str.append("(");
        if (query->left != nullptr) str.append(treeToString(query->left));
        if (query->right != nullptr) str.append(treeToString(query->right));
        str.append(")");
    }
    return str;
}


LRUCache::LRUCache() {
    cache.resize(0);
}

std::shared_ptr<SimpleGraph> LRUCache::getFromCache(RPQTree *query) {
    std::string queryStr = SimpleEvaluator::treeToString(query);
    std::shared_ptr<SimpleGraph> g = nullptr;

    for(int i = 0; i < cache.size(); i++) {
        std::pair<std::string, std::shared_ptr<SimpleGraph>> selected_pair = cache[i];
        if (queryStr == selected_pair.first){
            g = selected_pair.second;
            break;
        }
    }

    return g;
}

void LRUCache::addToCache(RPQTree *query, std::shared_ptr<SimpleGraph> &g) {
    std::string queryStr = SimpleEvaluator::treeToString(query);
    if (cache.size() < maxSize){
        cache.emplace_front(std::make_pair(queryStr, g));
    } else {
        cache.erase(cache.end());
        cache.emplace_front(std::make_pair(queryStr, g));
    }
}