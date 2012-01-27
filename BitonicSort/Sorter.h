/***
 * Sorter.h
 *
 *  Created on: Jul 4, 2011
 */

#ifndef SORTER_H_
#define SORTER_H_

#include <boost/serialization/vector.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi/collectives.hpp>
#include <boost/mpi/environment.hpp>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

const unsigned int RANDOM_ROUNDS = 2;
const unsigned int DATASET_SIZE = 16000000;
typedef boost::mpi::communicator communique;
typedef boost::mpi::environment environment;

class Sorter {
public:
	Sorter();
	virtual ~Sorter();

	void sayHello();
	bool isHeadNode();
	void generateDataSet();
	void distribteInitDataSet();
	void bitonicSort();
	bool isSorted();
	void gatherData();
	unsigned int getWorldSize();

private:
	unsigned int seed;
	unsigned int rank;
	unsigned int mask;
	unsigned int errors;
	unsigned int worldSize;
	unsigned int maxIteraions;
	unsigned int partnerRank;
	unsigned int windowId;
	std::vector <int> *initDataSet;
	std::vector <int> *localSet;
	std::vector <int> *remoteSet;
	std::vector <int> *unifiedSet;
	environment mpiEnvironment;
	communique mpiWorld;
	unsigned int getTag();
	void mergeDataSets(bool takeLow);
	void getWindowId(unsigned int index);
	void getPartnerRank(unsigned int iteration);
	void beginSort(unsigned int index);
	void compareLow();
	void compareHigh();
	bool isEven(unsigned int number);
	unsigned int getBitAt(unsigned int number, unsigned int shift);
};

#endif /* SORTER_H_ */
