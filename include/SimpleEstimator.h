//
// Created by Nikolay Yakovets on 2018-02-01.
//

#ifndef QS_SIMPLEESTIMATOR_H
#define QS_SIMPLEESTIMATOR_H

#include "Estimator.h"
#include "SimpleGraph.h"
#include <map>
#include <set>

enum EstimatorType {
	naive,
	simpleSampling,
	biasedSampling,
	radu,
	bodgan
};

class SimpleEstimator : public Estimator {

	// used by everyone
	std::shared_ptr<SimpleGraph> graph;
	EstimatorType estimatorType;

	// used by naive
	std::vector<uint32_t> incoming_labels;
	std::vector<uint32_t> outgoing_labels;
	uint32_t smaller_number_labels;
	uint32_t larger_number_labels;
	uint32_t length_query;

	// used by simple random sampling
	std::shared_ptr<SimpleGraph> biasedSampledGraph;
	float reduction_factor;

public:
	explicit SimpleEstimator(std::shared_ptr<SimpleGraph> &g);
	~SimpleEstimator() = default;

	void prepare() override;
	cardStat estimate(RPQTree *q) override;

	/** used by the naive estimator **/
	void prepareNaive();
	cardStat estimateNaive(RPQTree *q);
	void iterateRPQTree(RPQTree *q);
	bool onlyDigits(std::string str);

	/** used by random sampling estimator **/
	void prepForRandomSamplingEstimator(std::shared_ptr<SimpleGraph> &synopsis);
	void addEdgesByRandomWalk(std::shared_ptr<SimpleGraph> &synopsis,
							  float percentage_to_keep, int no_edges);
	cardStat estimateByRandomSampling(RPQTree *q);

	/** used by modified sampling estimator **/
	void prepForBiasedRandomSamplingEstimation();
	void removeEdgesByRandomWalk(std::shared_ptr<SimpleGraph> &synopsis,
								 float percentage_to_keep);
	cardStat estimateByBiasedRandomSampling(RPQTree *q);

	/** used by [name] **/
	void constructorRadu(std::shared_ptr<SimpleGraph> &g);
	void prepareRadu();
	cardStat estimateRadu(RPQTree *q);
	void destructorRadu();
	
	std::map<uint32_t, std::set<uint32_t>> bucketsAdj;
	std::map<uint32_t, std::set<uint32_t>> bucketsReverseAdj;
	std::shared_ptr<SimpleGraph> estimatedGraphRadu;

	template <typename T>
	std::pair<T, bool> getNthElement(std::set<T> & searchSet, int n);

	/** used by [name] **/
	void constructorBogdan(std::shared_ptr<SimpleGraph> &g);
	void prepareBogdan();
	cardStat estimateBogdan(RPQTree *q);
	void destructorBogdan();

};

#endif //QS_SIMPLEESTIMATOR_H