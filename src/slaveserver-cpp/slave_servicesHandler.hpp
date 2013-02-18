/*
 * SlaveServicesHander.hpp
 *
 *  Created on: Feb 14, 2013
 *      Author: alex
 */

#ifndef SLAVESERVICESHANDER_HPP_
#define SLAVESERVICESHANDER_HPP_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include "../gen-cpp/slave_services.h"
#include "boost/lexical_cast.hpp"

typedef int SOCKET;
#define SOCKET_ERROR	-1
#define INVALID_SOCKET	-1
#define CloseSocket(x)	(shutdown((x), SHUT_RDWR), close(x))

using namespace slave;
using namespace std;

class slave_servicesHandler: virtual public slave_servicesIf {
public:
	slave_servicesHandler() {
		// Your initialization goes here
	}

	bool ActiveStoreTransfer(const Params& p, const int32_t iflags) {

		volatile SOCKET DataSock;

		DataSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (DataSock != INVALID_SOCKET
				&& !setsockopt(DataSock, SOL_SOCKET, SO_RCVBUF,
						(char*) &p.transfer_buffer_size, sizeof(int))) {

			struct sockaddr_in BindSin;
			BindSin.sin_family = AF_INET;
			BindSin.sin_addr.s_addr = 16777343;

			unsigned short int usLen, usStart;
			usLen = 100;
			usStart = 900;
			//pFtpServer->GetDataPortRange(&usStart, &usLen);
			BindSin.sin_port = (unsigned short) (usStart + (rand() % usLen));

			int on = 1; // Here the strange behaviour of SO_REUSEADDR under win32 is welcome.

			(void) setsockopt(DataSock, SOL_SOCKET, SO_REUSEADDR, (char *) &on,
					sizeof on);

			if (bind(DataSock, (struct sockaddr *) &BindSin,
					sizeof(struct sockaddr_in)) != SOCKET_ERROR) {

				struct sockaddr_in ConnectSin;
				ConnectSin.sin_family = AF_INET;
				ConnectSin.sin_port = htons(p.client_port);
				ConnectSin.sin_addr.s_addr = inet_addr(p.client_ip.c_str());

				if (connect(DataSock, (struct sockaddr *) &ConnectSin,
						sizeof(ConnectSin)) == SOCKET_ERROR)
					return false;
			}

		}

		int len = 0;
		int hFile = -1;

		unsigned int uiBufferSize = p.transfer_buffer_size;
		char *pBuffer = new char[uiBufferSize];

		string file = slave_server::server.DataRootPath + "/"
				+ boost::lexical_cast<string>(p.fid);
		hFile = open(file.c_str(), iflags, (int) 0777);

		if (hFile >= 0) {
			if ((p.restart_at > 0 && lseek(hFile, p.restart_at, SEEK_SET) != -1)
					|| p.restart_at == 0) {
				fd_set fdRead;
				char *ps = pBuffer;
				char *pe = pBuffer + uiBufferSize;

				while (DataSock != INVALID_SOCKET) {

					FD_ZERO( &fdRead);
					FD_SET( DataSock, &fdRead);

					if (select(DataSock + 1, &fdRead, NULL, NULL, NULL)
							> 0&& FD_ISSET( DataSock, &fdRead )) {
						len = recv(DataSock, ps, (pe - ps), 0);
						if (len >= 0) {
							if (len > 0) {
								ps += len;
								if (ps == pe) {
									if (!SafeWrite(hFile, pBuffer,
											ps - pBuffer)) {
										len = -1;
										break;
									}
									ps = pBuffer;
								}
							} else {
								SafeWrite(hFile, pBuffer, ps - pBuffer);
								break;
							}
						} else
							break; // Socket Error

					} else {
						len = -1;
						break;
					}
				}
			}
			close(hFile);
		}
		printf("ActiveStoreTransfer\n");
		cout << file << endl;
		cout << len << endl;

		if (pBuffer)
			delete[] pBuffer;

		if (DataSock != INVALID_SOCKET) {
			SOCKET tmpSock = DataSock;
			DataSock = INVALID_SOCKET;
			CloseSocket(tmpSock);
		}

		return (len < 0 || hFile == -1) ? false : true;

	}

	bool PasvStoreTransfer(const Params& p, const int32_t iflags) {
		// Your implementation goes here
		printf("PasvStoreTransfer\n");
	}

	bool ActiveRetrieveTransfer(const Params& p) {
		// Your implementation goes here
		printf("ActiveRetrieveTransfer\n");
	}

	bool PasvRetrieveTransfer(const Params& p) {
		// Your implementation goes here
		printf("PasvRetrieveTransfer\n");
	}

private:
	bool SafeWrite(int hFile, char *pBuffer, int nLen) {
		int wl = 0, k;
		while (wl != nLen) {
			k = write(hFile, pBuffer + wl, nLen - wl);
			if (k < 0)
				return false;
			wl += k;
		}
		return true;
	}
}
;

#endif /* SLAVESERVICESHANDER_HPP_ */
