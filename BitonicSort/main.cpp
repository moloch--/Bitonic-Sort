/***
 * main.cpp
 *
 *  Created on: July 4, 2011
 *      Author: Moloch
 *    Compiler: mpic++ (GCC) 4.1.2 20080704 (Red Hat 4.1.2-50)
 *       About: Bitonic Sort using Boost MPI (v1.46.1)
 *
 */

#include <boost/timer.hpp>
#include <iostream>

#include "Sorter.h"

boost::timer timer;

void startSorting(Sorter &machine);
void checkSort(Sorter &machine);

int main(int argc, char* argv[]) {
	Sorter sortMachine;
    startSorting(sortMachine);
	if (sortMachine.isHeadNode()) {
		std::cout << "\033[1m[*] Sort completed in " << timer.elapsed() << " second(s)\033[0m" << std::endl;
		checkSort(sortMachine);
	}
	return 0;
}

void startSorting(Sorter &sortMachine) {
    if (sortMachine.isHeadNode()) {
        std::cout << "[+] I am the master process!" << std::endl;
        sortMachine.generateDataSet();
    } else {
        sortMachine.sayHello();
    }
    sortMachine.distribteInitDataSet();
    timer.restart();
    sortMachine.bitonicSort();
    sortMachine.gatherData();
}

void checkSort(Sorter &sortMachine) {
    if (sortMachine.isSorted()) {
        std::cout << "\033[1m\033[33m[$] BitonicSort completed successfully :D\033[0m" << std::endl;
    } else {
    	std::cout << "\033[1m[-] BitonicSort failed to sort all of the data\033[0m" << std::endl;
    }
}
