/*
 * slaveserver_main.h
 *
 *  Created on: Feb 11, 2013
 *      Author: alex
 */

#ifndef SLAVESERVER_MAIN_H_
#define SLAVESERVER_MAIN_H_

struct slaveserver_config {

	unsigned short int ThriftPort;
	int WorkerCount;
	std::string DataRootPath;
	std::string LogDirectory;

};

#endif /* SLAVESERVER_MAIN_H_ */
