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
#include "CFtpServer/CFtpServer.h"

void OnServerEvent(int Event) {
	switch (Event) {
	case CFtpServer::START_LISTENING:
		printf("* Server is listening !\r\n");
		break;

	case CFtpServer::START_ACCEPTING:
		printf("* Server is accepting incoming connections !\r\n");
		break;

	case CFtpServer::STOP_LISTENING:
		printf("* Server stopped listening !\r\n");
		break;

	case CFtpServer::STOP_ACCEPTING:
		printf("* Server stopped accepting incoming connections !\r\n");
		break;
	case CFtpServer::MEM_ERROR:
		printf(
				"* Warning, the CFtpServer class could not allocate memory !\r\n");
		break;
	case CFtpServer::THREAD_ERROR:
		printf(
				"* Warning, the CFtpServer class could not create a thread !\r\n");
		break;
	case CFtpServer::ZLIB_VERSION_ERROR:
		printf(
				"* Warning, the Zlib header version differs from the Zlib library version !\r\n");
		break;
	case CFtpServer::ZLIB_STREAM_ERROR:
		printf("* Warning, error during compressing/decompressing data !\r\n");
		break;
	}
}

void OnUserEvent(int Event, CFtpServer::CUserEntry *pUser, void *pArg) {
	switch (Event) {
	case CFtpServer::NEW_USER:
		printf(
				"* A new user has been created:\r\n"
						"\tLogin: %s\r\n" "\tPassword: %s\r\n" "\tStart directory: %s\r\n",
				pUser->GetLogin(), pUser->GetPassword(),
				pUser->GetStartDirectory());
		break;

	case CFtpServer::DELETE_USER:
		printf("* \"%s\"user is being deleted: \r\n", pUser->GetLogin());
		break;
	}
}

void OnClientEvent(int Event, CFtpServer::CClientEntry *pClient, void *pArg) {
	switch (Event) {
	case CFtpServer::NEW_CLIENT:
		printf("* A new client has been created:\r\n"
				"\tClient IP: [%s]\r\n\tServer IP: [%s]\r\n",
				inet_ntoa(*pClient->GetIP()),
				inet_ntoa(*pClient->GetServerIP()));
		break;

	case CFtpServer::DELETE_CLIENT:
		printf("* A client is being deleted.\r\n");
		break;

	case CFtpServer::CLIENT_AUTH:
		printf("* A client has logged-in as \"%s\".\r\n",
				pClient->GetUser()->GetLogin());
		break;

	case CFtpServer::CLIENT_SOFTWARE:
		printf("* A client has proceed the CLNT FTP command: %s.\r\n",
				(char*) pArg);
		break;

	case CFtpServer::CLIENT_DISCONNECT:
		printf("* A client has disconnected.\r\n");
		break;

	case CFtpServer::CLIENT_UPLOAD:
		printf("* A client logged-on as \"%s\" is uploading a file: \"%s\"\r\n",
				pClient->GetUser()->GetLogin(), (char*) pArg);
		break;

	case CFtpServer::CLIENT_DOWNLOAD:
		printf(
				"* A client logged-on as \"%s\" is downloading a file: \"%s\"\r\n",
				pClient->GetUser()->GetLogin(), (char*) pArg);
		break;

	case CFtpServer::CLIENT_LIST:
		printf(
				"* A client logged-on as \"%s\" is listing a directory: \"%s\"\r\n",
				pClient->GetUser()->GetLogin(), (char*) pArg);
		break;

	case CFtpServer::CLIENT_CHANGE_DIR:
		printf(
				"* A client logged-on as \"%s\" has changed its working directory:\r\n"
						"\tFull path: \"%s\"\r\n\tWorking directory: \"%s\"\r\n",
				pClient->GetUser()->GetLogin(), (char*) pArg,
				pClient->GetWorkingDirectory());
		break;

	case CFtpServer::RECVD_CMD_LINE:
		printf("* Received: %s (%s)>  %s\r\n",
				pClient->GetUser() ?
						pClient->GetUser()->GetLogin() : "(Not logged in)",
				inet_ntoa(*pClient->GetIP()), (char*) pArg);
		break;

	case CFtpServer::SEND_REPLY:
		printf("* Sent: %s (%s)>  %s\r\n",
				pClient->GetUser() ?
						pClient->GetUser()->GetLogin() : "(Not logged in)",
				inet_ntoa(*pClient->GetIP()), (char*) pArg);
		break;

	case CFtpServer::TOO_MANY_PASS_TRIES:
		printf("* Too many pass tries for (%s)\r\n",
				inet_ntoa(*pClient->GetIP()));
		break;
	}
}

int main(int argc, char * argv[]) {

	CFtpServer FtpServer;

	FtpServer.SetServerCallback(OnServerEvent);
	FtpServer.SetUserCallback(OnUserEvent);
	FtpServer.SetClientCallback(OnClientEvent);

	FtpServer.SetMaxPasswordTries(3);
	FtpServer.SetNoLoginTimeout(45); // seconds
	FtpServer.SetNoTransferTimeout(90); // seconds
	FtpServer.SetDataPortRange(100, 900); // data TCP-Port range = [100-999]
	FtpServer.SetCheckPassDelay(500); // milliseconds. Bruteforcing protection.

#ifdef CFTPSERVER_ENABLE_ZLIB
	FtpServer.EnableModeZ( true );
#endif


#ifdef CFTPSERVER_ENABLE_EXTRACMD // See "CFtpServer/config.h". not defined by default
		pUser->SetExtraCommand( CFtpServer::ExtraCmd_EXEC );
		// Security Warning ! Only here for example.
		// the last command allow the user to call the 'system()' C function!
#endif

		// If you only want to listen on the TCP Loopback interface,
		// replace 'INNADDR_ANY' by 'inet_addr("127.0.0.1")'.
		if (FtpServer.StartListening(INADDR_ANY, 2100)) {

			printf("-Server is listening ! :)\r\n");

			if (FtpServer.StartAccepting()) {

				printf("-Server successfully started ! :)\r\n");

				for (;;)
					sleep(1);

			} else
				printf("-Unable to accept incoming connections.\r\n");

			FtpServer.StopListening();

		} else
			printf("-Unable to listen.\r\n");


	printf("-Exiting.\r\n");

	return 0;
}
