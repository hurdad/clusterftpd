/*
 * metaserver_config.hpp
 *
 *  Created on: Feb 10, 2013
 *      Author: alex
 */

#ifndef METASERVER_CONFIG_HPP_
#define METASERVER_CONFIG_HPP_
#include "libconfig.h++"
#include <map>

namespace meta_server {
using namespace libconfig;

void InitServerConfig() {

	server.ListeningIP = "0.0.0.0"; //Listening Interface
	server.ListeningPort = 21; // By default, the FTP control port is 21
	server.CheckPassDelay = 500; // milliseconds. Bruteforcing protection.
	server.DataPortRange.usStart = 100; // TCP Ports [100;999].
	server.DataPortRange.usLen = 900;
	server.EnableFXP = true;
	server.MaxPasswordTries = 3;
	server.NoLoginTimeout = 0; // No timeout.
	server.NoTransferTimeout = 0; // No timeout.
	server.RedisIP = "127.0.0.1";
	server.RedisPort = 6379;
	server.RedisDB = 1;
	server.TransferBufferSize = 32 * 1024;
	server.TransferSocketBufferSize = 64 * 1024;

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

	cfg.lookupValue("ListeningIP", server.ListeningIP);
	int ListeningPort = server.ListeningPort;
	cfg.lookupValue("ListeningPort", ListeningPort);
	server.ListeningPort = ListeningPort;

	int Start = server.DataPortRange.usStart;
	int Len = server.DataPortRange.usLen;
	cfg.lookupValue("DataPortRange.Start", Start);
	cfg.lookupValue("DataPortRange.Len", Len);
	server.DataPortRange.usStart = Start;
	server.DataPortRange.usLen = Len;

	cfg.lookupValue("EnableFXP", server.EnableFXP);
	cfg.lookupValue("MaxPasswordTries", server.MaxPasswordTries);

	int NoLoginTimeout = server.NoLoginTimeout;
	int NoTransferTimeout = server.NoTransferTimeout;
	cfg.lookupValue("NoLoginTimeout", NoLoginTimeout);
	cfg.lookupValue("NoTransferTimeout", NoTransferTimeout);
	server.NoLoginTimeout = NoLoginTimeout;
	server.NoTransferTimeout = NoTransferTimeout;

	cfg.lookupValue("Redis.IP", server.RedisIP);
	int RedisPort = server.RedisPort;
	int RedisDB = server.RedisDB;
	cfg.lookupValue("Redis.Port", RedisPort);
	cfg.lookupValue("Redis.DB", RedisDB);
	server.RedisPort = RedisPort;
	server.RedisDB = RedisDB;

	cfg.lookupValue("TransferBufferSize", server.TransferBufferSize);
	cfg.lookupValue("TransferSocketBufferSize",
			server.TransferSocketBufferSize);

	if (cfg.exists("Slaves")) {
		Setting& slaves = cfg.lookup("Slaves");
		std::set<std::string> slave_map;
		for (int i = 0; i < slaves.getLength(); i++) {
			slave_map.insert(slaves[i]);
		}
		server.Slaves = slave_map;
	}
}

}
#endif /* METASERVER_CONFIG_HPP_ */
