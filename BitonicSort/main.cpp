/***
 * main.cpp
 *
 *  Created on: July 4, 2011
 *      Author: Moloch
 *    Compiler: mpic++ (GCC) 4.1.2 20080704 (Red Hat 4.1.2-50)
 *       About: Bitonic Sort using Boost MPI (v1.46.1)
 *
 *     Copyright (C) 2012  Moloch
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
