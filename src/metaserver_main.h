/*
 * metaserver_main.h
 *
 *  Created on: Feb 10, 2013
 *      Author: alex
 */

#ifndef METASERVER_MAIN_H_
#define METASERVER_MAIN_H_
#include <set>
extern struct metaserver_config server;
struct metaserver_config {

	std::string ListeningIP;
	unsigned short int ListeningPort;
	struct {
		unsigned short int usLen, usStart;
	} DataPortRange;

	unsigned int MaxPasswordTries;
	unsigned int CheckPassDelay;
	unsigned long int NoTransferTimeout, NoLoginTimeout;
	unsigned int TransferBufferSize, TransferSocketBufferSize;

#ifdef CFTPSERVER_ENABLE_ZLIB
	bool EnableZlib;
#endif
	bool EnableFXP;

	std::string  RedisIP;
	unsigned short RedisPort;
	unsigned short RedisDB;
	std::set<std::string> Slaves;

	std::string LogDirectory;
	bool EnableUserLogging;
	bool EnableClientLogging;
	bool EnableServerLogging;
};

#endif /* METASERVER_MAIN_H_ */
