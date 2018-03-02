//
// Created by Nikolay Yakovets on 2018-02-01.
//

#include "SimpleGraph.h"
#include "SimpleEstimator.h"
#include "SimpleEvaluator.h"


SimpleEstimator::SimpleEstimator(std::shared_ptr<SimpleGraph> &g) {
	// variables used by everyone
	graph = g;
	estimatorType = simpleSampling;


	// ************ variables used by jc's code ******
	// used for naive estimator implementation
	incoming_labels = std::vector<uint32_t>(0);
	outgoing_labels = std::vector<uint32_t >(0);

	// used for random sampling estimator

	// ************* variables used by radu's code ***********

	// ************* variables used by bodgan's code ************
}


void SimpleEstimator::prepare() {
	// do your prep here
	switch(estimatorType){
		case naive:
			prepareNaive();
			break;
		case simpleSampling:

			break;

		case radu:
			break;

		case bodgan:
			break;
	}
}


cardStat SimpleEstimator::estimate(RPQTree *q){
	cardStat result = cardStat{0,0,0};
	switch(estimatorType){
		case naive:
			result = estimateNaive(q);
		case simpleSampling:
			result = estimateByRandomSampling(q);
			break;
		case radu:
			break;
		case bodgan:
			break;
	}
	return result;
}



void SimpleEstimator::prepareNaive() {
	// do your prep here
	// play a bit with the graph data structure
	//std::cout << "Number of labels for the given graph: " << graph->getNoLabels() << std::endl;
	//std::cout << "Number of vertices for the given graph: " << graph->getNoVertices() << std::endl;
	//std::cout << "Number of edges for the given graph: " << graph->getNoEdges() << std::endl;

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

cardStat SimpleEstimator::estimateNaive(RPQTree *q) {
	q->print();
	// perform your estimation here

	// number of labels must be smaller or equal than the number of edges in the graph
	smaller_number_labels = graph->getNoEdges() + 1;
	larger_number_labels = 0;
	length_query = 0;
	// iterate

	//std::cout << "Before estimation process" << std::endl;
	//std::cout << "Smaller number of labels: " << smaller_number_labels << std::endl;
	//std::cout << "Larger number of labels: " << larger_number_labels << std::endl;

	// iterate over RPQTree data
	iterateRPQTree(q);
	//iterateRPQTree(q->righ)

	//std::cout << "After estimation process" << std::endl;
	//std::cout << "Smaller number of labels: " << smaller_number_labels << std::endl;
	//std::cout << "Larger number of labels: " << larger_number_labels << std::endl;

	// noOut, noPaths, noIn
	return cardStat{length_query, smaller_number_labels * larger_number_labels, length_query};
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

void SimpleEstimator::prepForRandomSamplingEstimator(std::shared_ptr<SimpleGraph> &synopsis){
	//std::cout << "Starting prepareForSimpleSamplingEstimator" << std::endl;
	float percentage_to_keep = 0.2;
	addEdgesByRandomWalk(synopsis, percentage_to_keep, graph->getNoEdges());
}

void SimpleEstimator::addEdgesByRandomWalk(std::shared_ptr<SimpleGraph> &synopsis,
										   float percentage_to_keep, int no_edges){
	//SimpleGraph synopsis = SimpleGraph();
	// force to resize the graph
	synopsis->setNoVertices(0);

	//std::cout << "Entering selectSubgraphByRandomWalk" << std::endl;
	uint32_t no_selected_edges = 0;
	uint32_t edges_to_select = std::floor(percentage_to_keep * no_edges);
	//std::cout << "Setting values for synopsis"<< std::endl;
	//synopsis.s
	synopsis->setNoVertices(graph->getNoVertices());
	synopsis->setNoLabels(graph->getNoLabels());

	//std::cout << "Number of edges: " << no_edges << std::endl;
	//std::cout << "Percentage of edges to keep: " << percentage_to_keep << std::endl;
	//std::cout << "Number of edges to select: " << edges_to_select << std::endl;

	std::srand(std::time(nullptr)); // use current time as seed for random generator
	int current_node = std::rand() % (graph->getNoVertices());

	// teleport parameter
	float alpha = 1 / graph->getNoVertices();

	// select random node to start the walking
	while (no_selected_edges < edges_to_select){

		if (std::rand() < alpha){
			// teleport by change
			current_node = std::rand() % (graph->getNoVertices());
		} else if (graph->adj[current_node].empty()){
			// force teleportation
			current_node = std::rand() % (graph->getNoVertices());
		} else if (synopsis->adj[current_node].size() >= graph->adj[current_node].size()){
			// force teleportation because we already processed all edges here
			current_node = std::rand() % (graph->getNoVertices());
		} else {
			// here we allow duplication
			int next_pair_pos = std::rand() % (graph->adj[current_node].size());
			std::pair<uint32_t, uint32_t> selected_pair = graph->adj[current_node][next_pair_pos];
			synopsis->addEdge(current_node, selected_pair.second, selected_pair.first);
			no_selected_edges++;
		}

	}

	//std::cout << "Number of selected edges: " << synopsis->getNoEdges() << std::endl;
	//std::cout << "Synopsis construction is finished" << std::endl;
}

cardStat SimpleEstimator::estimateByRandomSampling(RPQTree *q) {
	//std::cout << "Running simpleSamplingEstimator " << std::endl;
	auto synopsis = std::make_shared<SimpleGraph>();
	int reduction_factor = 3;
	float percentage_to_keep = 1.0 / reduction_factor;

	addEdgesByRandomWalk(synopsis, percentage_to_keep, graph->getNoEdges());
	//prepareForSimpleSamplingEstimator(synopsis);
	SimpleEvaluator eval = SimpleEvaluator(synopsis);
	cardStat firstStats = eval.evaluate(q);
	firstStats.noPaths = firstStats.noPaths*reduction_factor;
	// resize the synopsis object
	//delete &synopsis;

	addEdgesByRandomWalk(synopsis, percentage_to_keep, graph->getNoEdges());
	//prepareForSimpleSamplingEstimator(synopsis);
	eval = SimpleEvaluator(synopsis);
	cardStat secondStats = eval.evaluate(q);
	secondStats.noPaths = secondStats.noPaths*reduction_factor;

	uint32_t avgNoPaths = 0.5*(firstStats.noPaths + secondStats.noPaths);

	return cardStat{0, avgNoPaths, 0};
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


