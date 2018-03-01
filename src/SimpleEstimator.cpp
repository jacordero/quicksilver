//
// Created by Nikolay Yakovets on 2018-02-01.
//

#include "SimpleGraph.h"
#include "SimpleEstimator.h"

SimpleEstimator::SimpleEstimator(std::shared_ptr<SimpleGraph> &g) {

	// works only with SimpleGraph
	graph = g;
	labels = std::vector<uint32_t>(0);
	out = graph->getNoVertices();
	in = 0;
	//length_query

}

void SimpleEstimator::prepare() {

	// do your prep here
	// play a bit with the graph data structure
	std::cout << "Number of labels for the given graph: " << graph->getNoLabels() << std::endl;
	std::cout << "Number of vertices for the given graph: " << graph->getNoVertices() << std::endl;
	std::cout << "Number of edges for the given graph: " << graph->getNoEdges() << std::endl;

	int no_labels = graph->getNoLabels();

	// resize matrices

	labels.resize(no_labels);

	// fill matrices
	for (int i = 0; i < graph->adj.size(); i++)
	{
		auto adjElement = graph->adj[i];
		for (const auto &vlPair : adjElement) {
			uint8_t label = vlPair.first;
			auto outEl = vlPair.second;

			labels[label] = labels[label] + 1;
			in++;
		}
	}
}

cardStat SimpleEstimator::estimate(RPQTree *q) {

	q->print();
	// perform your estimation here

	// number of labels must be smaller or equal than the number of edges in the graph
	smaller_number_labels = graph->getNoEdges() + 1;
	larger_number_labels = 0;
	length_query = 0;
	// iterate

	std::cout << "Before estimation process" << std::endl;
	std::cout << "Smaller number of labels: " << smaller_number_labels << std::endl;
	std::cout << "Larger number of labels: " << larger_number_labels << std::endl;

	// iterate over RPQTree data
	iterateRPQTree(q);
	//iterateRPQTree(q->righ)

	std::cout << "After estimation process" << std::endl;
	std::cout << "Smaller number of labels: " << smaller_number_labels << std::endl;
	std::cout << "Larger number of labels: " << larger_number_labels << std::endl;

	// noOut, noPaths, noIn
	auto noOut = out / length_query;
	auto noIn = (in - out) / length_query;
	return cardStat{ noOut, smaller_number_labels * larger_number_labels, noIn };

	return cardStat{ 0, 0, 0 };
}


void SimpleEstimator::iterateRPQTree(RPQTree *q) {

	if (q->isLeaf()) {
		char c = (q->data).at(0);
		std::string str = std::string(1, c);
		if (onlyDigits(str)) {
			int label_id = std::stoi(str, nullptr);
			length_query++;
			int no_labels = labels[label_id];
			if (no_labels > larger_number_labels) {
				larger_number_labels = no_labels;
			}
			if (no_labels < smaller_number_labels) {
				smaller_number_labels = no_labels;
			}
		}
	}
	else {
		if (q->left != nullptr) iterateRPQTree(q->left);
		if (q->right != nullptr) iterateRPQTree(q->right);
	}
}

bool SimpleEstimator::onlyDigits(std::string str) {
	return (str.find_first_not_of("0123456789") == std::string::npos);
}