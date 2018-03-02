//
// Created by Nikolay Yakovets on 2018-02-01.
//

#ifndef QS_SIMPLEESTIMATOR_H
#define QS_SIMPLEESTIMATOR_H

#include "Estimator.h"
#include "SimpleGraph.h"

class SimpleEstimator : public Estimator {

	std::shared_ptr<SimpleGraph> graph;

	// matrices used to perform the cardinality estimation
	std::vector<uint32_t> incoming_labels;
	std::vector<uint32_t> outgoing_labels;
	uint32_t smaller_number_labels;
	uint32_t larger_number_labels;
	uint32_t length_query;


public:
	explicit SimpleEstimator(std::shared_ptr<SimpleGraph> &g);
	~SimpleEstimator() = default;

	void prepare() override;
	cardStat estimate(RPQTree *q) override;
	void iterateRPQTree(RPQTree *q);
	bool onlyDigits(std::string str);

	void constructorJorge(std::shared_ptr<SimpleGraph> &g);
	void prepareJorge();
	cardStat estimateJorge(RPQTree *q);
	void destructorJorge();

	void constructorRadu(std::shared_ptr<SimpleGraph> &g);
	void prepareRadu();
	cardStat estimateRadu(RPQTree *q);
	void destructorRadu();

	void constructorBogdan(std::shared_ptr<SimpleGraph> &g);
	void prepareBogdan();
	cardStat estimateBogdan(RPQTree *q);
	void destructorBogdan();

};

#endif //QS_SIMPLEESTIMATOR_H