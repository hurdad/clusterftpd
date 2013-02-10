/*
 * metaserver_log.hpp
 *
 *  Created on: Feb 10, 2013
 *      Author: alex
 */

#ifndef METASERVER_LOG_HPP_
#define METASERVER_LOG_HPP_

namespace meta_server{
class Log{

public:

	 void static OnServerEvent(int Event) {
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

	void static OnUserEvent(int Event, CFtpServer::CUserEntry *pUser, void *pArg) {
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

	void static OnClientEvent(int Event, CFtpServer::CClientEntry *pClient, void *pArg) {
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
};

}
#endif /* METASERVER_LOG_HPP_ */
