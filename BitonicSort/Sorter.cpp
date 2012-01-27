 /***
 * Sorter.cpp
 *
 *  Created on: Jul 4, 2011
 */

#include "Sorter.h"

Sorter::Sorter(): seed(0), errors(0) {
	rank = mpiWorld.rank();
	worldSize = mpiWorld.size();
	localSet = new std::vector <int>(DATASET_SIZE/worldSize);
	remoteSet = new std::vector <int>(DATASET_SIZE/worldSize);
	unifiedSet = new std::vector <int>((DATASET_SIZE/worldSize) * 2);
	if (isHeadNode()) {
		initDataSet = new std::vector <int>(DATASET_SIZE);
	} else {
		initDataSet = NULL;
	}
}

Sorter::~Sorter() {
	delete localSet;
	delete remoteSet;
	delete unifiedSet;
	if (initDataSet != NULL)
		delete initDataSet;
}

bool Sorter::isHeadNode() {
	return rank == 0 ? true:false;
}

void Sorter::sayHello() {
	std::cout << "[*] I am slave process (" << rank << ") of (" << worldSize - 1 << ")" << std::endl;
}

void Sorter::distribteInitDataSet() {
	mpiWorld.barrier();
	boost::mpi::scatter(this->mpiWorld, (*initDataSet), &(*localSet)[0], localSet->size(), 0);
}

void Sorter::bitonicSort() {
	std::sort(localSet->begin(), localSet->end());
	maxIteraions = (unsigned int) log2(worldSize);
	for (unsigned int iteration = 1; iteration <= maxIteraions; ++iteration) {
		for (int index = (iteration - 1); 0 <= index; --index) {
			seed++;
			getWindowId(maxIteraions - (maxIteraions - iteration));
			getPartnerRank(index);
			beginSort(index);
		}
	}
}

/* Hyper Cube */
void Sorter::beginSort(unsigned int index) {
	if ( (isEven(windowId) && getBitAt(rank, index) == 0) || ((!isEven(windowId)) && getBitAt(rank, index) == 1) ) {
		compareLow();
	} else {
		compareHigh();
	}
}

/* Returns 'n' most significant bits of rank, where n = bits */
void Sorter::getWindowId(unsigned int bits) {
	mask = 0x11111111;
	mask = mask << bits;
	windowId = (rank & mask) >> bits;
}

/* Calculate the rank of the node to trade with next */
void Sorter::getPartnerRank(unsigned int index) {
	if (((rank >> index) % 2) == 1) {
		partnerRank = (unsigned int) (rank - pow(2, index));
	} else {
		partnerRank = (unsigned int) (rank + pow(2, index));
	}
}

void Sorter::compareLow() {
	printf("\033[1m\033[36m[L]\033[0m  [%2d] -> [%2d] (%d)\n", rank, partnerRank, getTag());
	fflush(stdout);
	mpiWorld.recv(partnerRank, getTag(), remoteSet);
	mpiWorld.send(partnerRank, getTag()+1, localSet);
	mergeDataSets(true);
}

void Sorter::compareHigh() {
	printf("\033[1m\033[36m[H]\033[0m  [%2d] <- [%2d] (%d)\n", rank, partnerRank, getTag());
	fflush(stdout);
	mpiWorld.send(partnerRank, getTag(), localSet);
	mpiWorld.recv(partnerRank, getTag()+1, remoteSet);
	mergeDataSets(false);
}

unsigned int Sorter::getTag() {
	return ((partnerRank * rank) + (seed * worldSize));
}

/* Merge localSet and remoteSet, take the lower half if takeLow is true */
void Sorter::mergeDataSets(bool takeLow) {
	std::merge(localSet->begin(), localSet->end(), remoteSet->begin(), remoteSet->end(), unifiedSet->begin());
	std::vector <int>::iterator start;
	if (takeLow) {
		start = unifiedSet->begin();
	} else {
		start = unifiedSet->begin() + (localSet->size());
	}
	for (unsigned int index = 0; index < localSet->size(); index++) {
		localSet->at(index) = *start;
		start++;
	}
}

bool Sorter::isEven(unsigned int number) {
	return number % 2 == 0 ? true:false;
}

/* Returns bit at index in number */
unsigned int Sorter::getBitAt(unsigned int number, unsigned int index) {
	number = number >> index;
	return (number & 0x00000001);
}

/* Collects data from all nodes */
void Sorter::gatherData() {
	mpiWorld.barrier();
	if (isHeadNode()) {
		std::cout << "[*] Collecting data from all nodes." << std::endl;
	}
	if (isHeadNode()) {
		boost::mpi::gather(mpiWorld, &(*localSet)[0], localSet->size(), (*initDataSet), 0);
	} else {
		boost::mpi::gather(mpiWorld, &(*localSet)[0], localSet->size(), 0);
	}
}

/* Master Node Only */
void Sorter::generateDataSet() {
	std::cout << "[*] Allocating memory ..." << std::endl;
	for (unsigned int index = 0; index < DATASET_SIZE; ++index) {
		initDataSet->at(index) = index;
	}
	std::cout << "[*] Radomizing data set ..." << std::endl;
	for (unsigned int rounds = 0; rounds < RANDOM_ROUNDS; ++rounds) {
		std::random_shuffle(initDataSet->begin(), initDataSet->end());
	}
}

/* Master Node Only */
bool Sorter::isSorted() {
	if (initDataSet->size() != DATASET_SIZE) {
		std::cout << "\033[1m\033[31m[!] Failure:\033[0m Size of data set is not " << DATASET_SIZE << std::endl;
		return false;
	}
	for (unsigned int index = 1; index < initDataSet->size(); ++index) {
		if (initDataSet->at(index-1) > initDataSet->at(index)) {
			std::cout << "\033[1m\033[31m[!] Failure (" << index << "):\033[0m " << initDataSet->at(index-1);
			std::cout << " < " << initDataSet->at(index) << std::endl;
			errors++;
		}
	}
	return errors == 0 ? true:false;
}
