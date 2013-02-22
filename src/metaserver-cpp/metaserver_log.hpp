/*
 * metaserver_log.hpp
 *
 *  Created on: Feb 10, 2013
 *      Author: alex
 */

#ifndef METASERVER_LOG_HPP_
#define METASERVER_LOG_HPP_
#include <iostream>
#include <fstream>
using namespace std;

namespace meta_server {

class Log {

public:

	void static OnServerEvent(int Event) {

		//check config
		if (!server.EnableServerLogging)
			return;

		// Current date/time based on current system
		time_t now = time(0);

		// Convert now to tm struct for local timezone
		tm* localtm = localtime(&now);

		//string \n
		string temp = asctime(localtm);
		string ts = temp.substr(0, temp.size() - 1);

		// Init Log Stream
		string log_file = server.LogDirectory + "/metaserver.server.log";
		ofstream log(log_file.c_str(), ios::app);

		// Check for log
		if (!log.is_open()) {
			log.close();
			return;
		}

		switch (Event) {
		case CFtpServer::START_LISTENING:
			log << ts << " - Server is listening !\r\n";
			break;

		case CFtpServer::START_ACCEPTING:
			log << ts << " - Server is accepting incoming connections !\r\n";
			break;

		case CFtpServer::STOP_LISTENING:
			log << ts << " - Server stopped listening !\r\n";
			break;

		case CFtpServer::STOP_ACCEPTING:
			log << ts
					<< " - Server stopped accepting incoming connections !\r\n";

			break;
		case CFtpServer::MEM_ERROR:
			log << ts
					<< " - Warning, the CFtpServer class could not allocate memory !\r\n";

			break;
		case CFtpServer::THREAD_ERROR:
			log << ts
					<< " - Warning, the CFtpServer class could not create a thread !\r\n";

			break;
		case CFtpServer::ZLIB_VERSION_ERROR:
			log << ts
					<< " - Warning, the Zlib header version differs from the Zlib library version !\r\n";

			break;
		case CFtpServer::ZLIB_STREAM_ERROR:
			log << ts
					<< " - Warning, error during compressing/decompressing data !\r\n";

			break;
		case CFtpServer::THRIFT_ERROR:
				log << ts
						<< " - Warning, error thrift server exception !\r\n";

				break;
		}

		log.close();
	}

	void static OnUserEvent(int Event, CFtpServer::CUserEntry *pUser,
			void *pArg) {

		//check config
		if (!server.EnableServerLogging)
			return;

		// Current date/time based on current system
		time_t now = time(0);

		// Convert now to tm struct for local timezone
		tm* localtm = localtime(&now);

		//string \n
		string temp = asctime(localtm);
		string ts = temp.substr(0, temp.size() - 1);

		// Init Log Stream
		string log_file = server.LogDirectory + "/metaserver.user.log";
		ofstream log(log_file.c_str(), ios::app);

		// Check for log
		if (!log.is_open()) {
			log.close();
			return;
		}

		switch (Event) {
		case CFtpServer::NEW_USER:
			log << ts << " - A new user has been created:\r\n" << "\tLogin: "
					<< pUser->GetLogin() << "\r\n" "\tPassword: "
					<< pUser->GetPassword() << "\r\n" "\tStart directory: "
					<< pUser->GetStartDirectory() << "\r\n";
			break;

		case CFtpServer::DELETE_USER:
			log << ts << " - \"" << pUser->GetLogin()
					<< "\"user is being deleted: \r\n";

			break;
		}

		log.close();
	}

	void static OnClientEvent(int Event, CFtpServer::CClientEntry *pClient,
			void *pArg) {

		//check config
		if (!server.EnableClientLogging)
			return;

		//init
		string login;

		// Current date/time based on current system
		time_t now = time(0);

		// Convert now to tm struct for local timezone
		tm* localtm = localtime(&now);

		//string \n
		string temp = asctime(localtm);
		string ts = temp.substr(0, temp.size() - 1);

		// Init Log Stream
		string log_file = server.LogDirectory + "/metaserver.client.log";
		ofstream log(log_file.c_str(), ios::app);

		// Check for log
		if (!log.is_open()) {
			log.close();
			return;
		}

		switch (Event) {
		case CFtpServer::NEW_CLIENT:

			log << ts << " - A new client has been created:\r\n"
					<< "Client IP: [" << inet_ntoa(*pClient->GetIP())
					<< "]\r\n\tServer IP: ["
					<< inet_ntoa(*pClient->GetServerIP()) << "]\r\n";
			break;

		case CFtpServer::DELETE_CLIENT:
			log << ts << " - A client is being deleted.\r\n";
			break;

		case CFtpServer::CLIENT_AUTH:
			log << ts << " - A client has logged-in as \""
					<< pClient->GetUser()->GetLogin() << "\".\r\n";
			break;

		case CFtpServer::CLIENT_SOFTWARE:
			log << ts << " - A client has proceed the CLNT FTP command: "
					<< (char*) pArg << ".\r\n";
			break;

		case CFtpServer::CLIENT_DISCONNECT:
			log << ts << " - A client has disconnected.\r\n";
			break;

		case CFtpServer::CLIENT_UPLOAD:
			log << ts << " - A client logged-on as \""
					<< pClient->GetUser()->GetLogin()
					<< "\" is uploading a file: \"" << (char*) pArg << "\"\r\n";

			break;

		case CFtpServer::CLIENT_DOWNLOAD:
			log << ts << " - A client logged-on as \""
					<< pClient->GetUser()->GetLogin()
					<< "\" is downloading a file: \"" << (char*) pArg
					<< "\"\r\n";
			break;

		case CFtpServer::CLIENT_LIST:
			log << ts << " - A client logged-on as \""
					<< pClient->GetUser()->GetLogin()
					<< "\" is listing a directory: \"" << (char*) pArg
					<< "\"\r\n";
			break;

		case CFtpServer::CLIENT_CHANGE_DIR:
			log << ts << " - A client logged-on as \""
					<< pClient->GetUser()->GetLogin()
					<< "\" has changed its working directory:\r\n"
							"\tFull path: \"" << (char*) pArg
					<< "\"\r\n\tWorking directory: \""
					<< pClient->GetWorkingDirectory() << "\"\r\n";
			break;

		case CFtpServer::RECVD_CMD_LINE:
			login = pClient->GetUser() ?
					pClient->GetUser()->GetLogin() : "(Not logged in)";
			log << ts << " - Received: " << login << " ("
					<< inet_ntoa(*pClient->GetIP()) << ")>  " << (char*) pArg
					<< "\r\n";
			break;

		case CFtpServer::SEND_REPLY:
			login = pClient->GetUser() ?
					pClient->GetUser()->GetLogin() : "(Not logged in)";
			log << ts << " - Sent: " << login << " ("
					<< inet_ntoa(*pClient->GetIP()) << ")> " << (char*) pArg
					<< "\r\n";
			break;

		case CFtpServer::TOO_MANY_PASS_TRIES:
			log << ts << " - Too many pass tries for ("
					<< inet_ntoa(*pClient->GetIP()) << ")\r\n";
			break;
		}

		log.close();
	}
};

}
#endif /* METASERVER_LOG_HPP_ */
