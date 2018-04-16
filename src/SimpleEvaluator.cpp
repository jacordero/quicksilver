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
    index;
}

void SimpleEvaluator::attachEstimator(std::shared_ptr<SimpleEstimator> &e) {
    est = e;
}

void SimpleEvaluator::prepare() {

    // if attached, prepare the estimator
    if(est != nullptr) {
        /** Estimator is being prepared**/
        std::cout << "Estimator is being prepared\n";
        est->prepare();
    }

    // create the index
    //createIndex(graph->getNoLabels());

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
    //std::set<int> newStartVertices;

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
        //for (auto &mapEntry : in->reverse_adj) {
        for (int source: preselectedVertices){

            //uint32_t source = mapEntry.first;
            //auto sourceVec = mapEntry.second;
            auto sourceVec = in->reverse_adj[source];
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


std::shared_ptr<SimpleGraph> SimpleEvaluator::project_preselected_for_left_tree(uint32_t projectLabel,
                                                                                bool inverse,
                                                                                std::shared_ptr<SimpleGraph> &in,
                                                                                std::set<int> preselectedTargetVertices) {

    auto out = std::make_shared<SimpleGraph>(in->getNoVertices());
    out->setNoLabels(in->getNoLabels());

    //std::cout << "Preselected vertices size: " << preselectedVertices.size() << std::endl;
    //std::set<int> newTargetVertices;
    std::set<int> newStartVertices;

    if(!inverse) {
        // going forward
        //for (auto &mapEntry : in->adj) {
        for (int target: preselectedTargetVertices){

            //uint32_t source = mapEntry.first;
            //auto sourceVec = mapEntry.second;
            auto targetVec = in->reverse_adj[target];
            for (auto labelSource : targetVec) {

                auto label = labelSource.first;
                auto source = labelSource.second;

                if (label == projectLabel){
                    out->addEdge(source, target, label);
                    //newTargetVertices.insert(target);
                    newStartVertices.insert(source);
                }

            }
        }
    } else {
        // going backward
        // TODO: implement preselected vertices for backward mode
        //for (auto &mapEntry : in->reverse_adj) {
        for (int target: preselectedTargetVertices){

            //uint32_t source = mapEntry.first;
            //auto sourceVec = mapEntry.second;
            auto targetVec = in->adj[target];
            for (auto labelSource : targetVec) {

                auto label = labelSource.first;
                auto source = labelSource.second;

                if (label == projectLabel){
                    out->addEdge(source, target, label);
                    //newTargetVertices.insert(target);
                    newStartVertices.insert(source);
                }
            }
        }
    }

    out->setStartVertices(newStartVertices);
    //out->setStartVertices(newTargetVertices);
    return out;
}


std::shared_ptr<SimpleGraph> SimpleEvaluator::project_preselected_for_right_tree(uint32_t projectLabel,
                                                                                 bool inverse,
                                                                                 std::shared_ptr<SimpleGraph> &in,
                                                                                 std::set<int> preselectedEndVertices) {

    auto out = std::make_shared<SimpleGraph>(in->getNoVertices());
    out->setNoLabels(in->getNoLabels());

    //std::cout << "Preselected vertices size: " << preselectedVertices.size() << std::endl;
    std::set<int> newEndVertices;
    std::set<int> newStartVertices;
    //std::set<int> newStartVertices;

    if(!inverse) {
        // going forward
        //for (auto &mapEntry : in->adj) {
        for (int source: preselectedEndVertices){

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
        //for (auto &mapEntry : in->reverse_adj) {
        for (int source: preselectedEndVertices){

            //uint32_t source = mapEntry.first;
            //auto sourceVec = mapEntry.second;
            auto sourceVec = in->reverse_adj[source];
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

std::shared_ptr<SimpleGraph> SimpleEvaluator::smart_join(std::shared_ptr<SimpleGraph> &left, std::shared_ptr<SimpleGraph> &right, int depth) {

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
    std::shared_ptr<SimpleGraph> g = nullptr;// = cache.getFromCache(q);
    /**
    if (g != nullptr){
        //std::cout << "Cache hit: " << treeToString(q) <<"\n";
        return g;
    }
     **/

    if(q->isLeaf()) {
        //std::cout  <<"DEBUG: " << std::string(depth, '\t') << "Is leaf: " << q->data << std::endl;

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
        /*
        if (g->adj.size() == 0){
            return nullptr;
        }
         */
        return g;
    }

    if(q->isConcat()) {



        //std::cout << "DEBUG: " << std::string(depth, '\t') << "Is concat: " << q->data << std::endl;

        // evaluate the children
        //std::cout << "DEBUG: " << std::string(depth, '\t') << "left: ";
        //(q->left)->print();
        //std::cout << "DEBUG: " << "\n";
        //std::cout << "DEBUG: " << std::string(depth, '\t') << "right: ";
        //(q->right)->print();
        //std::cout << "DEBUG: " << std::string(depth, '\t') << "\n";


        auto leftGraph = SimpleEvaluator::evaluate_aux_preselected(q->left, preselectedVertices, depth+1);

        /*
        if (leftGraph == nullptr || leftGraph->adj.size() == 0){
            return nullptr;
        }
         */

        auto rightGraph = SimpleEvaluator::evaluate_aux_preselected(q->right, leftGraph->endVertices, depth + 1);
        /*
        if (rightGraph == nullptr || rightGraph->adj.size() == 0){
            return nullptr;
        }
         */

        //std::cout  << std::string(depth, '\t') << "Graph vertices: " << graph->getNoVertices() << std::endl;
        //std::cout  << std::string(depth, '\t') << "Left graph vertices: " << leftGraph->adj.size() << std::endl;
        //std::cout  << std::string(depth, '\t') << "Left graph end vertices: " << leftGraph->getEndVertices().size() << std::endl;
        //std::cout  << std::string(depth, '\t') << "Right graph vertices: " << rightGraph->adj.size() << std::endl;


        // join left with right
        g = SimpleEvaluator::join(leftGraph, rightGraph, depth);
        //g->printGraph();
        //g->printStartVertices();
        //g->printEndVertices();
        //cache.addToCache(q, g);
        return g;
    }

    return nullptr;
}


std::shared_ptr<SimpleGraph> SimpleEvaluator::evaluate_preselected_for_left_tree(RPQTree *q,
                                                                                 std::set<int> preselectedSourceVertices,
                                                                                 int depth) {

    // evaluate according to the AST bottom-up
    std::shared_ptr<SimpleGraph> g = nullptr;// = cache.getFromCache(q);
    /**
    if (g != nullptr){
        //std::cout << "Cache hit: " << treeToString(q) <<"\n";
        return g;
    }
     **/

    if(q->isLeaf()) {
        //std::cout << "DEBUG: " << std::string(depth, '\t') << "Is leaf: " << q->data << std::endl;

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

        g = SimpleEvaluator::project_preselected_for_left_tree(label, inverse, graph, preselectedSourceVertices);
        //cache.addToCache(q, g);
        /*
        if (g->adj.size() == 0){
            return nullptr;
        }
         */
        //std::cout << "DEBUG: "  << std::string(depth, '\t') << "projected graph start vertices: " << g->startVertices.size() << std::endl;

        return g;
    }

    if(q->isConcat()) {



        //std::cout << "DEBUG: " << std::string(depth, '\t') << "Is concat: " << q->data << std::endl;

        // evaluate the children
        //std::cout << std::string(depth, '\t') << "DEBUG: left: ";
        //(q->left)->print();
        //std::cout << "\n";
        //std::cout << std::string(depth, '\t') << "DEBUG: right: ";
        //(q->right)->print();
        //std::cout << std::string(depth, '\t') << "\n";


        auto rightGraph = SimpleEvaluator::evaluate_preselected_for_left_tree(q->right, preselectedSourceVertices, depth+1);

        /*
        if (leftGraph == nullptr || leftGraph->adj.size() == 0){
            return nullptr;
        }
         */

        auto leftGraph = SimpleEvaluator::evaluate_preselected_for_left_tree(q->left, rightGraph->startVertices, depth + 1);
        /*
        if (rightGraph == nullptr || rightGraph->adj.size() == 0){
            return nullptr;
        }
         */

        //std::cout  << std::string(depth, '\t') << "Graph vertices: " << graph->getNoVertices() << std::endl;
        //std::cout  << std::string(depth, '\t') << "Left graph vertices: " << leftGraph->adj.size() << std::endl;
        //std::cout  << std::string(depth, '\t') << "Left graph end vertices: " << leftGraph->getEndVertices().size() << std::endl;
        //std::cout  << std::string(depth, '\t') << "Right graph vertices: " << rightGraph->adj.size() << std::endl;


        // join left with right
        g = SimpleEvaluator::join(leftGraph, rightGraph, depth);
        //std::cout  << std::string(depth, '\t') << "DEBUG: joined graph start vertices: " << g->startVertices.size() << std::endl;

        //g->printGraph();
        //g->printStartVertices();
        //g->printEndVertices();
        //cache.addToCache(q, g);
        return g;
    }

    return nullptr;
}


std::shared_ptr<SimpleGraph> SimpleEvaluator::evaluate_preselected_for_right_tree(RPQTree *q,
                                                                                  std::set<int> preselectedVertices,
                                                                                  int depth) {
    // evaluate according to the AST bottom-up
    std::shared_ptr<SimpleGraph> g = nullptr;// = cache.getFromCache(q);
    /**
    if (g != nullptr){
        //std::cout << "Cache hit: " << treeToString(q) <<"\n";
        return g;
    }
     **/

    if(q->isLeaf()) {
        //std::cout  << std::string(depth, '\t') << "DEBUG: Is leaf: " << q->data << std::endl;

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

        g = SimpleEvaluator::project_preselected_for_right_tree(label, inverse, graph, preselectedVertices);
        //std::cout  << std::string(depth, '\t') << "DEBUG: projected graph end vertices: " << g->endVertices.size() << std::endl;

        //cache.addToCache(q, g);
        /*
        if (g->adj.size() == 0){
            return nullptr;
        }
         */
        return g;
    }

    if(q->isConcat()) {



        //std::cout << std::string(depth, '\t') << "DEBUG: Is concat: " << q->data << std::endl;

        // evaluate the children
        //std::cout << std::string(depth, '\t') << "DEBUG: left: ";
        //(q->left)->print();
        //std::cout << "\n";
        //std::cout << std::string(depth, '\t') << "DEBUG: right: ";
        //(q->right)->print();
        //std::cout << std::string(depth, '\t') << "\n";


        auto leftGraph = SimpleEvaluator::evaluate_preselected_for_right_tree(q->left, preselectedVertices, depth+1);

        /*
        if (leftGraph == nullptr || leftGraph->adj.size() == 0){
            return nullptr;
        }
         */

        auto rightGraph = SimpleEvaluator::evaluate_preselected_for_right_tree(q->right, leftGraph->endVertices, depth + 1);
        /*
        if (rightGraph == nullptr || rightGraph->adj.size() == 0){
            return nullptr;
        }
         */

        //std::cout  << std::string(depth, '\t') << "DEBUG: Graph vertices: " << graph->getNoVertices() << std::endl;
        //std::cout  << std::string(depth, '\t') << "DEBUG: Left graph vertices: " << leftGraph->adj.size() << std::endl;
        //std::cout  << std::string(depth, '\t') << "DEBUG: Left graph end vertices: " << leftGraph->getEndVertices().size() << std::endl;
        //std::cout  << std::string(depth, '\t') << "DEBUG: Right graph vertices: " << rightGraph->adj.size() << std::endl;


        // join left with right
        g = SimpleEvaluator::join(leftGraph, rightGraph, depth);
        //std::cout  << std::string(depth, '\t') << "DEBUG: joined graph end vertices: " << g->endVertices.size() << std::endl;

        //g->printGraph();
        //g->printStartVertices();
        //g->printEndVertices();
        //cache.addToCache(q, g);
        return g;
    }

    return nullptr;
}


cardStat SimpleEvaluator::evaluate(RPQTree *query) {
    std::cout <<"\n";
    //auto res = evaluate_aux(query, 0);
    cardStat estimation = est->estimate(query);

    // clean query
    std::string queryStr = treeToString(query);
    // remove (, ), and /
    queryStr.erase(std::remove(queryStr.begin(), queryStr.end(), '('), queryStr.end());
    queryStr.erase(std::remove(queryStr.begin(), queryStr.end(), ')'), queryStr.end());
    queryStr.erase(std::remove(queryStr.begin(), queryStr.end(), '/'), queryStr.end());
    //std::cout << "cleaned query string: " << queryStr << std::endl;
    //std::cout << "qs[0]: " << queryStr[0] << ", qs[1]: " << queryStr[1] << ", qs[2]: " << queryStr[2] << ", qs[3]: " << queryStr[3]<< std::endl;

    std::vector<std::string> tokens = extractTokens(queryStr);
    int pivotPosition = findPivotIndex(tokens);
    //std::cout << "pivot index position: " << pivotPosition << std::endl;
    //std::cout << "pivot index value: " << tokens[pivotPosition] << std::endl;

    bool useNewEvaluationMethod = true;

    if (pivotPosition == tokens.size() - 1){
        //std::cout << "Execute plan for case 1" << std::endl;

        std::string strForTree = "";
        int limit = tokens.size() - 2;
        for (int i = 0; i < limit; i++){
            strForTree += tokens[i];
            strForTree += "/";
        }
        strForTree += tokens[limit];

        // create query treeRPQTree::strToTree(query.path)
        RPQTree* rightTreeQuery = RPQTree::strToTree(tokens[tokens.size() - 1]);
        auto rightGraph = evaluate_aux_preselected(rightTreeQuery, graph->endVertices, 0);
        //std::cout << "Right graph size: " << rightGraph->getNoEdges() << std::endl;
        //std::cout << "Right graph start vertices: " << rightGraph->startVertices.size() << std::endl;

        RPQTree* leftTreeQuery = RPQTree::strToTree(strForTree);
        auto leftGraph = evaluate_preselected_for_left_tree(leftTreeQuery, rightGraph->startVertices, 0);

        auto finalGraph = join(leftGraph, rightGraph, 0);
        cardStat stats = SimpleEvaluator::computeStats(finalGraph);
        //std::cout << "Actual (noOut, noPaths, noIn) : (" << stats.noOut << ", " << stats.noPaths << ", " << stats.noIn << ")\n";
        return stats;
    } else if (pivotPosition == 1) {
        //std::cout << "Execute old evaluation query procedure" << std::endl;

        std::string strForRightTree = "";
        for (int i = 2; i < tokens.size(); i++){
            strForRightTree += "(";
        }

        for (int i = 1; i < tokens.size() - 1; i++){

            strForRightTree += tokens[i];
            if (i >= 2){
                strForRightTree += ")";
            }
            strForRightTree += "/";
        }
        strForRightTree += tokens[tokens.size() - 1];
        strForRightTree += ")";
        //std::cout << "string for right query: " + strForRightTree << std::endl;

        RPQTree* leftTreeQuery = RPQTree::strToTree(tokens[0]);
        auto leftGraph = evaluate_aux_preselected(leftTreeQuery, graph->endVertices, 0);

        RPQTree* rightTreeQuery = RPQTree::strToTree(strForRightTree);
        auto rightGraph = evaluate_preselected_for_right_tree(rightTreeQuery, leftGraph->endVertices, 0);

        //std::cout << "Final left graph size: " << leftGraph->getNoEdges() << std::endl;
        //std::cout << "Final left graph end vertices: " << leftGraph->endVertices.size() << std::endl;
        //std::cout << "Final right graph size: " << rightGraph->getNoEdges() << std::endl;
        //std::cout << "Final right graph start vertices: " << rightGraph->startVertices.size() << std::endl;

        auto finalGraph = join(leftGraph, rightGraph, 0);
        //std::cout << "start compute stats: " << std::endl;
        cardStat stats = SimpleEvaluator::computeStats(finalGraph);
        //std::cout << "end compute stats: " << std::endl;
        //std::cout << "Actual (noOut, noPaths, noIn) : (" << stats.noOut << ", " << stats.noPaths << ", " << stats.noIn << ")\n";
        return stats;

    } else if (useNewEvaluationMethod) {

        //std::cout << "DEBUG: Execute plan for case 3" << std::endl;

        RPQTree* middleQueryTree = RPQTree::strToTree(tokens[pivotPosition]);

        std::string strForLeftTree = "";
        int limit = tokens.size() - 2;
        for (int i = 0; i < pivotPosition - 1; i++){
            strForLeftTree += tokens[i];
            strForLeftTree += "/";
        }
        strForLeftTree += tokens[pivotPosition - 1];
        //std::cout << "DEBUG: string for left query: " + strForLeftTree << std::endl;
        RPQTree* leftQueryTree = RPQTree::strToTree(strForLeftTree);

        // construct string for right query
        std::string strForRightTree = "";
        for (int i = pivotPosition + 1; i < tokens.size() - 1; i++){
            strForRightTree += "(";
        }

        for (int i = pivotPosition + 1; i < tokens.size() - 1; i++){
            strForRightTree += tokens[i];
            if (i >= pivotPosition + 2){
                strForRightTree += ")";
            }
            strForRightTree += "/";
        }
        strForRightTree += tokens[tokens.size() - 1];
        strForRightTree += ")";
        //std::cout << "DEBUG: string for right query: " + strForRightTree << std::endl;
        RPQTree* rightQueryTree = RPQTree::strToTree(strForRightTree);

        auto middleGraph = evaluate_aux_preselected(middleQueryTree, graph->endVertices, 0);
        auto leftGraph = evaluate_preselected_for_left_tree(leftQueryTree, middleGraph->startVertices, 0);
        auto rightGraph = evaluate_preselected_for_right_tree(rightQueryTree, middleGraph->endVertices, 0);

        //std::cout << "DEBUG: Left graph size: " << leftGraph->getNoEdges() << std::endl;
        //std::cout << "DEBUG: Middle graph size: " << middleGraph->getNoEdges() << std::endl;
        //std::cout << "DEBUG: Right graph size: " << rightGraph->getNoEdges() << std::endl;

        auto lmGraph = join(leftGraph, middleGraph, 0);
        auto finalGraph = join(lmGraph, rightGraph, 0);

        cardStat stats = SimpleEvaluator::computeStats(finalGraph);
        //std::cout << "Actual (noOut, noPaths, noIn) : (" << stats.noOut << ", " << stats.noPaths << ", " << stats.noIn << ")\n";
        return stats;
    } else {
        //std::cout << "DEBUG: Execute old evaluation query procedure" << std::endl;

        //std::cout << "DEBUG: Estimated paths: " << estimation.noPaths << std::endl;
        auto res = evaluate_aux_preselected(query, graph->endVertices, 0);
        return SimpleEvaluator::computeStats(res);
    }
}


int SimpleEvaluator::findPivotIndex(std::vector<std::string> tokens){
    int pivot = 0;
    int paths = 0;
    for (int i = 0; i < tokens.size(); i++){
        uint32_t tokenPaths = est->cardinalityEstimatorTable[tokens[i]];
        if (i == 0){
            paths = tokenPaths;
        } else if (tokenPaths < paths) {
            paths = tokenPaths;
            pivot = i;
        }
    }
    return pivot;
}

std::vector<std::string> SimpleEvaluator::extractTokens(std::string queryStr){
    std::vector<std::string> tokens;
    std::string currentToken = "";
    for (int i = 0; i < queryStr.length(); i++){
        currentToken += queryStr[i];
        if (queryStr[i] == '+' || queryStr[i] == '-'){
            tokens.emplace_back(currentToken);
            currentToken = "";
        }
    }

    return tokens;
}

std::string SimpleEvaluator::treeToStringForTokens(RPQTree *query){
    std::string str;
    if (query->left == nullptr && query->right == nullptr){
        str.append(query->data);
        str.append(" ");
    } else {
        str.append("(");
        if (query->left != nullptr) str.append(treeToString(query->left));
        if (query->right != nullptr) str.append(treeToString(query->right));
        str.append(")");
    }
    return str;
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


// TODO: reimplement the LRU cache class to make look up easier
// add a hashtable to keep the query and graphs and let the deque keeps the order
// of deletion for the graphs and queries
std::shared_ptr<SimpleGraph> LRUCache::getFromCache(RPQTree *query) {
    std::string queryStr = SimpleEvaluator::treeToString(query);
    // remove unnecessary brackets

    //std::cout << "Query before normalization: " << queryStr;
    queryStr.erase(std::remove(queryStr.begin(), queryStr.end(), '('), queryStr.end());
    queryStr.erase(std::remove(queryStr.begin(), queryStr.end(), ')'), queryStr.end());
    //std::cout << "Query after normalization: " << queryStr;

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

    //std::cout << "Query before normalization: " << queryStr;
    queryStr.erase(std::remove(queryStr.begin(), queryStr.end(), '('), queryStr.end());
    queryStr.erase(std::remove(queryStr.begin(), queryStr.end(), ')'), queryStr.end());
    //std::cout << "Query after normalization: " << queryStr;

    if (cache.size() < maxSize){
        cache.emplace_front(std::make_pair(queryStr, g));
    } else {
        cache.erase(cache.end());
        cache.emplace_front(std::make_pair(queryStr, g));
    }
}


SimpleIndex::SimpleIndex() {};

std::shared_ptr<SimpleGraph> SimpleIndex::getFromIndex(std::string key) {
    std::map<std::string, std::shared_ptr<SimpleGraph>>::iterator it;
    it = index.find(key);
    if (it != index.end()) {
        return it->second;
    } else {
        return nullptr;
    }
}

void SimpleIndex::addToIndex(std::string key, std::shared_ptr<SimpleGraph> &g) {
    index[key] = g;
}

void SimpleEvaluator::createIndex(int numberLabels){
    std::map<std::string, int> labelCount;

    for (int i = 0; i < numberLabels; i++){
        std::string label = std::to_string(i);
        std::string fLabel = label + "+";
        RPQTree* ftree = RPQTree::strToTree(fLabel);
        auto resf = evaluate_aux(ftree, 0);
        auto stats = SimpleEvaluator::computeStats(resf);
        labelCount[fLabel] = stats.noPaths;

        std::string bLabel = label + "-";
        RPQTree* btree = RPQTree::strToTree(fLabel);
        auto resb = evaluate_aux(btree, 0);
        stats = SimpleEvaluator::computeStats(resb);
        labelCount[bLabel] = stats.noPaths;
    }

    /**
    for (auto pair: labelCount){
        std::cout << "label: " + pair.first + ", count: " << pair.second << std::endl;
    }
     **/
}