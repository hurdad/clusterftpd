/*
 * metaserver_config.hpp
 *
 *  Created on: Feb 10, 2013
 *      Author: alex
 */

#ifndef METASERVER_CONFIG_HPP_
#define METASERVER_CONFIG_HPP_
#include "libconfig.h++"
#include <iostream>

namespace meta_server {

using namespace libconfig;

metaserver_config server;

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
	server.LogDirectory = "logs";
	server.EnableUserLogging = false;
	server.EnableClientLogging = true;
	server.EnableServerLogging = true;

}

void LoadServerConfig(const char * configFile) {
	Config cfg;

	// Read the file. If there is an error, report it and exit.
	try {
		cfg.readFile(configFile);
	} catch (const FileIOException &fioex) {
		std::cerr << "Error while reading config file: " << configFile
				<< std::endl;
		exit (EXIT_FAILURE);
	} catch (const ParseException &pex) {
		std::cerr << "Configuration parse error at " << pex.getFile() << ":"
				<< pex.getLine() << " - " << pex.getError() << std::endl;
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
		for (int i = 0; i < slaves.getLength(); i++) {

			slave_info myinfo;
			const char* host = slaves[i]["host"];
			string shost(host);
			myinfo.host = shost;
			myinfo.port = (int) slaves[i]["port"];
			;

			server.Slaves.push_back(myinfo);
		}
	}

	cfg.lookupValue("LogDirectory", server.LogDirectory);
	cfg.lookupValue("EnableUserLogging", server.EnableUserLogging);
	cfg.lookupValue("EnableClientLogging", server.EnableClientLogging);
	cfg.lookupValue("EnableServerLogging", server.EnableServerLogging);

}

void InitRedis() {

	//connect
	redisContext *c = redisConnect(server.RedisIP.c_str(), server.RedisPort);
	if (c != NULL && c->err) {
		cout << "Redis Error: " << c->errstr << endl;
		return;
	}

	//select database
	redisReply* reply = (redisReply*) redisCommand(c, "SELECT %u",
			server.RedisDB);
	if (reply != NULL && c->err) {
		cout << "Redis Error: " << c->errstr << endl;
		return;
	}
	freeReplyObject(reply);

	cout
			<< "Warning!: This will wipe ALL user and virtual filesystem data. Continue? (y/n)"
			<< endl;
	string input;
	cin >> input;
	while (true) {

		if (input == "n")
			return;

		if (input == "y")
			break;

		cout << "Invalid response (y/n)" << endl;
		cin >> input;
	}

	//wipe db
	redisCommand(c, "FLUSHDB");

	//init counter
	redisCommand(c, "SET uid_counter 1");
	redisCommand(c, "SET gid_counter 1");
	redisCommand(c, "HSET group:1 STAFF");
	redisCommand(c, "SET fid_counter 0");
	redisCommand(c, "SET transaction_counter 0");

	//add admin user
	redisCommand(c, "HSET username:admin password admin");
	redisCommand(c, "HSET username:admin start_directory /");
	redisCommand(c, "HSET username:admin enabled true");
	redisCommand(c, "HSET username:admin uid 1");
	redisCommand(c, "HSET username:admin gid 1");
	redisCommand(c, "HSET username:admin max_logins 0");
	redisCommand(c, "HSET username:admin privileges ?");

	//root fs
	redisCommand(c, "HSET dirent:0 .foo -1");

	//root folder
	redisCommand(c, "HSET stat:0 st_mode 16895");
	redisCommand(c, "HSET stat:0 st_size 4096");
	redisCommand(c, "HSET stat:0 st_uid 1");
	redisCommand(c, "HSET stat:0 st_gid 1");
	redisCommand(c, "HSET stat:0 st_mtime %u", time(NULL));

	redisFree(c);

	cout << "Init Complete" << endl;
}

}
#endif /* METASERVER_CONFIG_HPP_ */
