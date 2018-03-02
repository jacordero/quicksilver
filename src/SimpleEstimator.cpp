//
// Created by Nikolay Yakovets on 2018-02-01.
//

#include "SimpleGraph.h"
#include "SimpleEstimator.h"

SimpleEstimator::SimpleEstimator(std::shared_ptr<SimpleGraph> &g) {
	constructorJorge(g);
}

void SimpleEstimator::constructorJorge(std::shared_ptr<SimpleGraph> &g)
{
	// works only with SimpleGraph
	graph = g;
	incoming_labels = std::vector<uint32_t>(0);
	outgoing_labels = std::vector<uint32_t >(0);
	//length_query
}

void SimpleEstimator::prepareJorge()
{
	// do your prep here
	// play a bit with the graph data structure
	std::cout << "Number of labels for the given graph: " << graph->getNoLabels() << std::endl;
	std::cout << "Number of vertices for the given graph: " << graph->getNoVertices() << std::endl;
	std::cout << "Number of edges for the given graph: " << graph->getNoEdges() << std::endl;

	int no_labels = graph->getNoLabels();

	// resize matrices
	incoming_labels.resize(no_labels);
	outgoing_labels.resize(no_labels);

	// fill matrices
	int from = 0;
	for (const auto & adjElement : graph->adj) {
		for (const auto &vlPair : adjElement) {
			uint8_t label = vlPair.first;
			incoming_labels[label] = incoming_labels[label] + 1;
			outgoing_labels[label] = outgoing_labels[label] + 1;
		}
		//std::cout << "From: " << from << ", Label: " << vlPair.first << ", To: " << vlPair.second << std::endl;

		//from++;
	}

}

cardStat SimpleEstimator::estimateJorge(RPQTree *q)
{
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
	return cardStat{ length_query, smaller_number_labels * larger_number_labels, length_query };

	return cardStat{ 0, 0, 0 };
}

void SimpleEstimator::destructorJorge()
{

}

void SimpleEstimator::constructorRadu(std::shared_ptr<SimpleGraph> &g)
{

}

void SimpleEstimator::prepareRadu()
{

}

cardStat SimpleEstimator::estimateRadu(RPQTree *q)
{
	return cardStat{ 0, 0, 0 };
}

void SimpleEstimator::destructorRadu()
{

}

void SimpleEstimator::constructorBogdan(std::shared_ptr<SimpleGraph> &g)
{

}

void SimpleEstimator::prepareBogdan()
{

}

cardStat SimpleEstimator::estimateBogdan(RPQTree *q)
{
	return cardStat{ 0, 0, 0 };
}

void SimpleEstimator::destructorBogdan()
{

}

void SimpleEstimator::prepare() {
	prepareJorge();
}

cardStat SimpleEstimator::estimate(RPQTree *q) {
	return estimateJorge(q);
}


void SimpleEstimator::iterateRPQTree(RPQTree *q) {

	if (q->isLeaf()) {
		char c = (q->data).at(0);
		std::string str = std::string(1, c);
		if (onlyDigits(str)) {
			int label_id = std::stoi(str, nullptr);
			length_query++;
			int no_labels = incoming_labels[label_id];
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