//
// Created by Nikolay Yakovets on 2018-02-01.
//

#include "SimpleGraph.h"
#include "SimpleEstimator.h"
#include "SimpleEvaluator.h"


SimpleEstimator::SimpleEstimator(std::shared_ptr<SimpleGraph> &g) {
	// variables used by everyone
	graph = g;
	estimatorType = tables;
    //cardinalityEstimatorTable

	// ************ variables used by jc's code ******
	// used for naive estimator implementation



	incoming_labels = std::vector<uint32_t>(0);
	outgoing_labels = std::vector<uint32_t >(0);

	// used for random sampling estimator
	percentage_to_keep = 0.2;

	// used for random sampling estimator
	reduction_factor = 2.5;

	// ************* variables used by radu's code ***********
	constructorRadu(g);
	// ************* variables used by bodgan's code ************
}


void SimpleEstimator::prepare() {
	// do your prep here
	switch(estimatorType){

        case tables:
            prepareTables();
            break;
        case naive:
			prepareNaive();
			break;
		case simpleSampling:
			prepForRandomSamplingEstimator();
			break;
		case biasedSampling:
			prepForBiasedRandomSamplingEstimation();
			break;
		case radu:
			prepareRadu();
			break;

		case bodgan:
			break;
	}
}


cardStat SimpleEstimator::estimate(RPQTree *q){
	cardStat result = cardStat{0,0,0};
	switch(estimatorType){
        case tables:
            result = estimateUsingTables(q);
            break;
		case naive:
			result = estimateNaive(q);
		case simpleSampling:
			result = estimateByRandomSampling(q);
			break;
		case biasedSampling:
			result = estimateByBiasedRandomSampling(q);
			break;
		case radu:
			result = estimateRadu(q);
			break;
		case bodgan:
			break;
	}
	return result;
}


#pragma region Jorge

void SimpleEstimator::prepareTables(){
    int noLabels = graph->getNoLabels();
	//graph->getNoEdges();

    // initialize values for the cardinality estimator table
    for (int i = 0; i < noLabels; i++){
        std::string outgoingLabel = std::to_string(i);
        std::string incomingLabel = std::to_string(i);
        outgoingLabel += '+';
        incomingLabel += '-';
        cardinalityEstimatorTable[outgoingLabel] = 0;
        cardinalityEstimatorTable[incomingLabel] = 0;
    }


        // fill outgoing
    for (const auto & adjElement : graph->adj){
        for (const auto &vlPair : adjElement.second){
            uint32_t intLabel = vlPair.first;
            std::string strLabel = std::to_string(intLabel);
            strLabel += '+';
            cardinalityEstimatorTable[strLabel] = cardinalityEstimatorTable[strLabel] + 1;
        }
    }

        // fill incoming
    for (const auto & radjElement : graph->reverse_adj){
        for (const auto &vlPair : radjElement.second){
            uint32_t intLabel = vlPair.first;
            std::string strLabel = std::to_string(intLabel);
            strLabel += '-';
            cardinalityEstimatorTable[strLabel] = cardinalityEstimatorTable[strLabel] + 1;
        }
    }

	/**
    for (std::map<std::string,uint32_t>::iterator it=cardinalityEstimatorTable.begin();
         it!=cardinalityEstimatorTable.end(); ++it){
        std::cout << it->first << " => " << it->second << '\n';
    }
    **/

}



cardStat SimpleEstimator::estimateUsingTables(RPQTree *q) {

    //std::cout <<"passed tree: ";
    //q->print();
    //std::cout <<"\n";
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
	cardStat estimation = traverseRPQTree(q);
	//iterateRPQTree(q->righ)

	//std::cout << "After estimation process" << std::endl;
	//std::cout << "Smaller number of labels: " << smaller_number_labels << std::endl;
	//std::cout << "Larger number of labels: " << larger_number_labels << std::endl;

	// noOut, noPaths, noIn
	return estimation;
}


cardStat SimpleEstimator::traverseRPQTree(RPQTree *q) {
    //std::cout << "traverse rpq tree\n";
	if (q->isLeaf()) {
        //std::cout << "is leaf\n";
		char c = (q->data).at(0);
		std::string str = std::string(1, c);
		if (onlyDigits(str)) {
			//std::cout << "label: " << q->data << std::endl;
			//length_query++;
			int paths = cardinalityEstimatorTable[q->data];
            return cardStat{0, paths, 0};
		}
	}
	else {
        //std::cout <<"is inner node\n";

        cardStat leftEstimation = cardStat{0, 0, 0};
        cardStat rightEstimation = cardStat{0, 0, 0};
		if (q->left != nullptr){
            leftEstimation.noPaths = traverseRPQTree(q->left).noPaths;
        }
        if (q->right != nullptr){
            rightEstimation.noPaths = traverseRPQTree(q->right).noPaths;
        }

        uint32_t estimatedPaths = static_cast<uint32_t > (std::round((rightEstimation.noPaths + leftEstimation.noPaths) / 2.0 ));

        cardStat estimation = cardStat{0, estimatedPaths, 0};
        //std::cout << "Right paths: " << rightEstimation.noPaths << std::endl;
        //std::cout << "Left paths: " << leftEstimation.noPaths << std::endl;
		//std::cout << "Estimated paths: " << estimatedPaths << std::endl;

        return estimation;
	}
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
		for (const auto &vlPair : adjElement.second) {
			uint32_t label = vlPair.first;
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
	return cardStat{ length_query, smaller_number_labels * larger_number_labels, length_query };
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

void SimpleEstimator::prepForRandomSamplingEstimator() {
	//std::cout << "Starting prepareForSimpleSamplingEstimator" << std::endl;
	float percentage_to_keep = 0.2;
	firstSimpleSampledGraph = std::make_shared<SimpleGraph>();
	firstSimpleSampledGraph->setNoVertices(graph->getNoVertices());
	firstSimpleSampledGraph->setNoLabels(graph->getNoLabels());

	addEdgesByRandomWalk(firstSimpleSampledGraph, percentage_to_keep, graph->getNoEdges());

	secondSimpleSampledGraph = std::make_shared<SimpleGraph>();
	secondSimpleSampledGraph->setNoVertices(graph->getNoVertices());
	secondSimpleSampledGraph->setNoLabels(graph->getNoLabels());

	addEdgesByRandomWalk(firstSimpleSampledGraph, percentage_to_keep, graph->getNoEdges());

}

void SimpleEstimator::addEdgesByRandomWalk(std::shared_ptr<SimpleGraph> &synopsis,
	float percentage_to_keep, int no_edges) {
	//SimpleGraph synopsis = SimpleGraph();
	// force to resize the graph

	//std::cout << "Entering selectSubgraphByRandomWalk" << std::endl;
	uint32_t no_selected_edges = 0;
	uint32_t edges_to_select = std::floor(percentage_to_keep * no_edges);
	//std::cout << "Setting values for synopsis"<< std::endl;
	//synopsis

	//std::cout << "Number of edges: " << no_edges << std::endl;
	//std::cout << "Percentage of edges to keep: " << percentage_to_keep << std::endl;
	//std::cout << "Number of edges to select: " << edges_to_select << std::endl;

	std::srand(std::time(nullptr)); // use current time as seed for random generator
	int current_node = std::rand() % (graph->getNoVertices());

	// teleport parameter
	float alpha = 0.2;

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
	//prepareForSimpleSamplingEstimator(synopsis);
	SimpleEvaluator eval = SimpleEvaluator(firstSimpleSampledGraph);
	cardStat firstStats = eval.evaluate(q);
	firstStats.noPaths = firstStats.noPaths*(1.0/percentage_to_keep);
	// resize the synopsis object
	//delete &synopsis;

	//prepareForSimpleSamplingEstimator(synopsis);
	eval = SimpleEvaluator(secondSimpleSampledGraph);
	cardStat secondStats = eval.evaluate(q);
	secondStats.noPaths = secondStats.noPaths*(1.0/percentage_to_keep);

	uint32_t avgNoPaths = 0.5*(firstStats.noPaths + secondStats.noPaths);

	return cardStat{0, avgNoPaths, 0};
}

void SimpleEstimator::prepForBiasedRandomSamplingEstimation() {
	//std::cout << "entering prepForBiasedRandomSamplingEstimation " << std::endl;
	biasedSampledGraph = std::make_shared<SimpleGraph>();
	biasedSampledGraph->setNoVertices(graph->getNoVertices());
	biasedSampledGraph->setNoLabels(graph->getNoLabels());

	// first make a copy of the original graph
	//std::cout << "Making a copy of original graph" << std::endl;
	for (uint32_t fromVertex = 0; fromVertex < graph->getNoVertices(); fromVertex++) {
		for (auto labelTarget : graph->adj[fromVertex]) {
			int target = labelTarget.second;
			int label = labelTarget.first;
			biasedSampledGraph->addEdge(fromVertex, target, label);
		}
	}


	removeEdgesByRandomWalk(biasedSampledGraph, 1.0 / reduction_factor);
}

void SimpleEstimator::removeEdgesByRandomWalk(std::shared_ptr<SimpleGraph> &synopsis,
	float percentage_to_keep) {


	//std::cout << "Entering removeEdgesByRandomWalk" << std::endl;
	uint32_t no_selected_edges = 0;
	uint32_t edges_to_keep = std::floor(percentage_to_keep * synopsis->getNoEdges());

	std::srand(std::time(nullptr)); // use current time as seed for random generator
	int current_node = std::rand() % (synopsis->getNoVertices());

	// teleport parameter
	float alpha = 0.1;

	// select random node to start the walking
	while (synopsis->getNoEdges() > edges_to_keep){
		//std::cout << "Edges to keep: " << edges_to_keep << std::endl;
		//std::cout << "Number of edges: " << synopsis->getNoEdges() << std::endl;
		if (std::rand() < alpha){
			// teleport by change
			current_node = std::rand() % (synopsis->getNoVertices());
		} else if (synopsis->adj[current_node].size() <= 1){
			// force teleportation to avoid destroying weak links
			current_node = std::rand() % (synopsis->getNoVertices());
		} else {
			uint32_t next_pair_pos = std::rand() % (synopsis->adj[current_node].size());
			std::pair<uint32_t, uint32_t> selected_pair = synopsis->adj[current_node][next_pair_pos];
			// move current node to target
			int next_node = selected_pair.second;
			// remove the edge
			synopsis->adj[current_node].erase(synopsis->adj[current_node].begin() + next_pair_pos);
			current_node = next_node;
		}

	}
	//std::cout << "Edges to keep: " << edges_to_keep << std::endl;
	//std::cout << "Synopsis edges: " << synopsis->getNoEdges() << std::endl;
}

cardStat SimpleEstimator::estimateByBiasedRandomSampling(RPQTree *q) {
	//std::cout << "Running estimatebyBiasedRandomSampling " << std::endl;
	//prepareForSimpleSamplingEstimator(synopsis);
	SimpleEvaluator eval = SimpleEvaluator(biasedSampledGraph);
	cardStat firstStats = eval.evaluate(q);
	firstStats.noPaths = firstStats.noPaths*reduction_factor;
	return firstStats;
	// resize the synopsis object
	//delete &synopsis;

	/**
    addEdgesByRandomWalk(synopsis, percentage_to_keep, graph->getNoEdges());
    //prepareForSimpleSamplingEstimator(synopsis);
    eval = SimpleEvaluator(synopsis);
    cardStat secondStats = eval.evaluate(q);
    secondStats.noPaths = secondStats.noPaths*reduction_factor;

    uint32_t avgNoPaths = 0.5*(firstStats.noPaths + secondStats.noPaths);

    return cardStat{0, avgNoPaths, 0};
     **/
}
#pragma endregion

#pragma region Radu
void SimpleEstimator::constructorRadu(std::shared_ptr<SimpleGraph> &g)
{
	//bucketsAdj.resize(graph->getNoLabels());
	//bucketsReverseAdj.resize(graph->getNoLabels());
}

void SimpleEstimator::prepareRadu()
{
	srand(time(NULL));
	uint32_t sumBucketAdj = 0, sumBucketReverseAdj = 0;

	//construct bucket for adj list
	for (const auto & adjElement : graph->adj) {
		for (const auto &vlPair : adjElement.second) {
			uint8_t label = vlPair.first;
			uint32_t node = vlPair.second;
			bucketsAdj[label].insert(node);
		}
	}

	//sum the values of histogram for adj
	for (auto i = 0; i < bucketsAdj.size(); i++)
	{
		sumBucketAdj += bucketsAdj[i].size();
	}

	//construct bucket for reverse_adj list
	for (const auto & adjElement : graph->reverse_adj) {
		for (const auto &vlPair : adjElement.second) {
			uint8_t label = vlPair.first;
			uint32_t node = vlPair.second;
			bucketsReverseAdj[label].insert(node);
		}
	}

	//sum the values of histogram for reverse_adj
	for (auto i = 0; i < bucketsReverseAdj.size(); i++)
	{
		sumBucketReverseAdj += bucketsReverseAdj[i].size();
	}

	//get a ratio(ceil) based on the sum of two buckets / # vertices.
	//auto divison = (float)(sumBucketAdj + sumBucketReverseAdj) / graph->getNoVertices();
	uint32_t ratio = std::ceil(7);

	//construct an estimated graph based on buckets and ratio.
	estimatedGraphRadu = std::make_shared<SimpleGraph>();
	estimatedGraphRadu->setNoVertices(graph->getNoVertices());
	estimatedGraphRadu->setNoLabels(graph->getNoLabels());

	for (auto i = 0; i < estimatedGraphRadu->getNoVertices(); i++) {
		for (auto j = 0; j < ratio; j++)
		{
			int randIndex = rand() % bucketsAdj.size();
			int randIndexList = rand() % bucketsAdj[randIndex].size();
			
			auto target = getNthElement(bucketsAdj[randIndex], randIndexList);
			estimatedGraphRadu->addEdge(i, target.first , randIndex);
		}	
	}
}

template <typename T>
std::pair<T, bool> SimpleEstimator::getNthElement(std::set<T> & searchSet, int n)
{
	std::pair<T, bool> result;
	if (searchSet.size() > n)
	{
		result.first = *(std::next(searchSet.begin(), n));
		result.second = true;
	}
	else
		result.second = false;

	return result;
}

cardStat SimpleEstimator::estimateRadu(RPQTree *q)
{
	SimpleEvaluator eval = SimpleEvaluator(estimatedGraphRadu);
	return eval.evaluate(q);
}

void SimpleEstimator::destructorRadu()
{

}

#pragma endregion

#pragma region Bogdan

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
#pragma endregion