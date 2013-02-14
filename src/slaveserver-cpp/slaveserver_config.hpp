/*
 * slaveserver_config.hpp
 *
 *  Created on: Feb 11, 2013
 *      Author: alex
 */

#ifndef SLAVESERVER_CONFIG_HPP_
#define SLAVESERVER_CONFIG_HPP_
#include "libconfig.h++"

namespace slave_server {
using namespace libconfig;

slaveserver_config server;
void InitServerConfig() {

	server.ThriftPort = 9090;
	server.WorkerCount = 50;
	server.DataRootPath = "data";
	server.LogDirectory = "logs";
}

void LoadServerConfig(char * configFile) {
	Config cfg;

	// Read the file. If there is an error, report it and exit.
	try {
		cfg.readFile(configFile);
	} catch (const FileIOException &fioex) {
		std::cerr << "I/O error while reading config file." << std::endl;
		exit (EXIT_FAILURE);
	} catch (const ParseException &pex) {
		std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
				<< " - " << pex.getError() << std::endl;
		exit (EXIT_FAILURE);
	}

	int ThriftPort = server.ThriftPort;
	cfg.lookupValue("ThriftPort", ThriftPort);
	server.ThriftPort = ThriftPort;
	cfg.lookupValue("WorkerCount", server.WorkerCount);
	cfg.lookupValue("DataRootPath", server.DataRootPath);
	cfg.lookupValue("LogDirectory", server.LogDirectory);

}

}

#endif /* SLAVESERVER_CONFIG_HPP_ */
