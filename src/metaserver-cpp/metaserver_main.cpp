/*
 * CFtpServer :: a FTP Server Class Library
 *
 * Mail :: thebrowser@gmail.com

 Copyright (C) 2007 Julien Poumailloux

 This software is provided 'as-is', without any express or implied
 warranty.  In no event will the authors be held liable for any damages
 arising from the use of this software.

 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it
 freely, subject to the following restrictions:

 1. The origin of this software must not be misrepresented; you must not
 claim that you wrote the original software. If you use this software
 in a product, an acknowledgment in the product documentation would be
 appreciated but is not required.
 2. Altered source versions must be plainly marked as such, and must not be
 misrepresented as being the original software.
 3. This notice may not be removed or altered from any source distribution.
 4. If you like this software, a fee would be appreciated, particularly if
 you use this software in a commercial product, but is not required.

 GCC Compile line:
 g++ ./CFtpServer/CFtpServer.cpp ./Example/main.cpp -o FtpServer -Wall -lpthread -D_REENTRANT
 Under SunOS and mayber some other OS, you may need to link to '-lsocket', or '-lnsl' too.
 Add '-D_FILE_OFFSET_BITS=64' for large file support.

 */

#define CFTPSERVER_CONFIG_H_PATH "CFtpServerConfig.h"
#include <boost/program_options.hpp>
#include "CFtpServer/CFtpServer.h"
#include "metaserver_main.h"
#include "metaserver_config.hpp"
#include "metaserver_log.hpp"

using namespace meta_server;
using namespace std;
using namespace boost::program_options;

int main(int argc, char * argv[]) {

	string config;

	options_description desc("Options");
	desc.add_options()("help", "Options related to the program.")("config,c",
			value<string>(&config)->default_value("metaserver-sample.cfg"),
			"Configuration File")("daemon,d", "Daemon Mode")("init,i",
			"Initialization Mode ");

	variables_map vm;
	try {
		store(parse_command_line(argc, argv, desc), vm);
		notify(vm);
	} catch (exception &e) {
		cout << e.what() << endl;
		return EXIT_FAILURE;
	}

	//print help
	if (vm.count("help")) {
		cout << desc << endl;
		return EXIT_SUCCESS;
	}

	//init config
	InitServerConfig();
	LoadServerConfig(config.c_str());

	//Modes
	if (vm.count("init")) {
		InitRedis();
	}

	if (!vm.count("daemon")) {
		return EXIT_SUCCESS;
	}

	//init ftp object
	CFtpServer FtpServer;

	//set logging call backs
	FtpServer.SetServerCallback(Log::OnServerEvent);
	FtpServer.SetUserCallback(Log::OnUserEvent);
	FtpServer.SetClientCallback(Log::OnClientEvent);

	//configuration
	FtpServer.SetMaxPasswordTries(server.MaxPasswordTries);
	FtpServer.SetNoLoginTimeout(server.NoLoginTimeout); // seconds
	FtpServer.SetNoTransferTimeout(server.NoTransferTimeout); // seconds
	FtpServer.SetDataPortRange(server.DataPortRange.usStart,
			server.DataPortRange.usLen); // data TCP-Port range = [100-999]
	FtpServer.SetCheckPassDelay(server.CheckPassDelay); // milliseconds. Bruteforcing protection.
	FtpServer.SetTransferBufferSize(server.TransferBufferSize);
	FtpServer.SetTransferSocketBufferSize(server.TransferSocketBufferSize);
	FtpServer.EnableFXP(server.EnableFXP);
	FtpServer.SetRedisConnectionConfig(server.RedisIP, server.RedisPort, server.RedisDB);
	FtpServer.SetSlaves(server.Slaves);

#ifdef CFTPSERVER_ENABLE_ZLIB
	FtpServer.EnableModeZ( true );
#endif

#ifdef CFTPSERVER_ENABLE_EXTRACMD // See "CFtpServer/config.h". not defined by default
	pUser->SetExtraCommand( CFtpServer::ExtraCmd_EXEC );
	// Security Warning ! Only here for example.
	// the last command allow the user to call the 'system()' C function!
#endif

	//Start listening
	if (FtpServer.StartListening(inet_addr(server.ListeningIP.c_str()),
			server.ListeningPort)) {

		if (FtpServer.StartAccepting()) {

			//loop
			for (;;)
				sleep(1);

		}

		//shutdown
		FtpServer.StopListening();

	} else {
		Log::OnServerEvent(CFtpServer::ERROR_LISTENING);
	}

	return 0;
}

