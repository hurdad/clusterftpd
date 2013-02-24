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
#include <map>

typedef int SOCKET;
#define SOCKET_ERROR	-1
#define INVALID_SOCKET	-1
#define O_BINARY 0
#define CloseSocket(x)	(shutdown((x), SHUT_RDWR), close(x))

using namespace slave;
using namespace std;

class slave_servicesHandler: virtual public slave_servicesIf {
public:
	slave_servicesHandler() {
		mDataSock = new map<int64_t, SOCKET>();
	}

	void ActiveStoreTransfer(StorRetVal& _return, const ActiveParams& p,
			const int32_t iflags) {

		if (!OpenActiveDataConnection(CMD_STOR, p.transfer_buffer_size, p)) {
			_return.msg = "425 Can't open data connection.";
			_return.size = -1;
			return;
		}

		Store(_return, p.transfer_buffer_size, p.fid, p.restart_at, iflags,
				p.trans_id);
	}

	void PasvStoreTransfer(StorRetVal& _return, const PasvParams& p,
			const int32_t iflags) {

		Store(_return, p.transfer_buffer_size, p.fid, p.restart_at, iflags,
				p.trans_id);
	}

	void ActiveRetrieveTransfer(std::string& _return, const ActiveParams& p) {

		if (!OpenActiveDataConnection(CMD_RETR, p.transfer_buffer_size, p)) {
			_return = "425 Can't open data connection.";
			return;
		}

		Retrieve(_return, p.transfer_buffer_size, p.fid, p.restart_at,
				p.trans_id);
	}

	void PasvRetrieveTransfer(std::string& _return, const PasvParams& p) {

		Retrieve(_return, p.transfer_buffer_size, p.fid, p.restart_at,
				p.trans_id);
	}

	void InitPasvDataConnection(std::string& _return, const PasvParams& p,
			const int32_t startPort, const int16_t portLen) {

		(*mDataSock)[p.trans_id] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		struct sockaddr_in sin;
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = p.server_ip;
#ifdef USE_BSDSOCKETS
		sin.sin_len = sizeof( sin );
#endif

		int on = 1;

		if ((*mDataSock)[p.trans_id] != INVALID_SOCKET
#ifndef WIN32
				&& setsockopt((*mDataSock)[p.trans_id], SOL_SOCKET,
						SO_REUSEADDR, (char *) &on, sizeof(on)) != SOCKET_ERROR
#endif
		) {

			unsigned short server_port = 0;
			unsigned short int usLen = portLen;
			unsigned short int usStart = startPort;
			unsigned short int usFirstTry = (unsigned short) (usStart
					+ (rand() % usLen));
			unsigned short usTriedPort = usFirstTry;
			for (;;) {
				sin.sin_port = htons( usTriedPort );
				if (!bind((*mDataSock)[p.trans_id], (struct sockaddr *) &sin,
						sizeof(struct sockaddr_in))) {
					server_port = usTriedPort;
					break;
				}
				--usTriedPort;
				if (usTriedPort < usFirstTry)
					usTriedPort = (unsigned short int) (usStart + usLen);
				if (usTriedPort == usFirstTry) {
					break;
				}
			}
			if (!server_port) {
				_return = "451 Internal error - No more data port available.";
				return;
			}
			if (listen((*mDataSock)[p.trans_id], 1) == SOCKET_ERROR) {
				_return = "451 Internal error - Unable to listen.";
				return;
			}

			unsigned long ulIp = ntohl(p.server_ip);

			char buffer[50];
			sprintf(buffer, "227 Entering Passive Mode (%lu,%lu,%lu,%lu,%u,%u)",
					(ulIp >> 24) & 255, (ulIp >> 16) & 255, (ulIp >> 8) & 255,
					ulIp & 255, server_port / 256, server_port % 256);

			_return = string(buffer);

		}
	}

	void OpenPasvDataConnection(std::string& _return, const PasvParams& p) {

		SOCKET sTmpWs;
		struct sockaddr_in sin;
		socklen_t sin_len = sizeof(struct sockaddr_in);
#ifdef USE_BSDSOCKETS
		sin.sin_len = sizeof( sin );
#endif

		fd_set fdRead;
		FD_ZERO( &fdRead);
		FD_SET( (*mDataSock)[p.trans_id], &fdRead);
		timeval tv;
		tv.tv_sec = 10;
		tv.tv_usec = 0;

		if (select((*mDataSock)[p.trans_id] + 1, &fdRead, NULL, NULL, &tv)
				> 0&& FD_ISSET( (*mDataSock)[p.trans_id], &fdRead )) {
			sTmpWs = accept((*mDataSock)[p.trans_id], (struct sockaddr *) &sin,
					&sin_len);
			CloseSocket( (*mDataSock)[p.trans_id]);
			(*mDataSock)[p.trans_id] = sTmpWs;
			if (sTmpWs != INVALID_SOCKET) {
				_return = "150 Connection accepted.";
				return;
			}

			ResetDataConnection(p.trans_id);
			_return = "425 Can't open data connection.";
		}
	}

private:
	struct stat st;
	enum {
		CMD_NONE = -1,
		CMD_QUIT,
		CMD_USER,
		CMD_PASS,
		CMD_NOOP,
		CMD_ALLO,
		CMD_SITE,
		CMD_HELP,
		CMD_SYST,
		CMD_STRU,
		CMD_MODE,
		CMD_TYPE,
		CMD_CLNT,
		CMD_PORT,
		CMD_PASV,
		CMD_LIST,
		CMD_NLST,
		CMD_CWD,
		CMD_XCWD,
		CMD_FEAT,
		CMD_MDTM,
		CMD_PWD,
		CMD_XPWD,
		CMD_CDUP,
		CMD_XCUP,
		CMD_STAT,
		CMD_ABOR,
		CMD_REST,
		CMD_RETR,
		CMD_STOR,
		CMD_APPE,
		CMD_STOU,
		CMD_SIZE,
		CMD_DELE,
		CMD_RNFR,
		CMD_RNTO,
		CMD_MKD,
		CMD_XMKD,
		CMD_RMD,
		CMD_XRMD,
		CMD_OPTS,
		CMD_PRET
	};

	map<int64_t, SOCKET> *mDataSock;

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

	void Retrieve(std::string& _return, const int32_t transfer_buffer_size,
			const int64_t fid, const int64_t restart_at,
			const int64_t trans_id) {

		int hFile = -1;
		int BlockSize = 0;
		int len = 0;

		unsigned int uiBufferSize = transfer_buffer_size;
		char *pBuffer = new char[uiBufferSize];

		string file = slave_server::server.DataRootPath + "/"
				+ boost::lexical_cast<string>(fid);

		if (!pBuffer) {
			goto endofretrieve;
		}

		hFile = open(file.c_str(), O_RDONLY | O_BINARY);
		if (hFile != -1) {
			if (restart_at == 0
					|| (restart_at > 0
							&& lseek(hFile, restart_at, SEEK_SET) != -1)) {
				while ((*mDataSock)[trans_id] != INVALID_SOCKET && (BlockSize =
						read(hFile, pBuffer, uiBufferSize)) > 0) {

					{
						len = send((*mDataSock)[trans_id], pBuffer, BlockSize,
								MSG_NOSIGNAL);
						if (len <= 0)
							break;
					}
				}
			} // else Internal Error
			close(hFile);
		}

		endofretrieve: if (pBuffer)
			delete[] pBuffer;

		ResetDataConnection(trans_id);

		if (len < 0 || hFile == -1)
			_return = "550 Can't retrieve File.";
		else
			_return = "226 Transfer complete.";

	}

	void Store(StorRetVal& _return, const int32_t transfer_buffer_size,
			const int64_t fid, const int64_t restart_at, const int32_t iflags,
			const int64_t trans_id) {

		int len = 0;
		int hFile = -1;

		unsigned int uiBufferSize = transfer_buffer_size;
		char *pBuffer = new char[uiBufferSize];

		string file = slave_server::server.DataRootPath + "/"
				+ boost::lexical_cast<string>(fid);

		if (!pBuffer) {
			goto endofstore;
		}

		hFile = open(file.c_str(), iflags, (int) 0777);

		if (hFile >= 0) {
			if ((restart_at > 0 && lseek(hFile, restart_at, SEEK_SET) != -1)
					|| restart_at == 0) {
				fd_set fdRead;
				char *ps = pBuffer;
				char *pe = pBuffer + uiBufferSize;

				while ((*mDataSock)[trans_id] != INVALID_SOCKET) {

					FD_ZERO( &fdRead);
					FD_SET( (*mDataSock)[trans_id], &fdRead);

					if (select((*mDataSock)[trans_id] + 1, &fdRead, NULL, NULL,
							NULL)
							> 0&& FD_ISSET( (*mDataSock)[trans_id], &fdRead )) {
						len = recv((*mDataSock)[trans_id], ps, (pe - ps), 0);
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
		endofstore:

		if (pBuffer)
			delete[] pBuffer;

		ResetDataConnection(trans_id);

		if (len < 0 || hFile == -1)
			_return.msg = "550 Can't store file.";
		else
			_return.msg = "226 Transfer complete.";

		//stat file for size
		if (stat(file.c_str(), &st) == 0)
			_return.size = st.st_size;
		else
			_return.size = -1;
	}

	bool OpenActiveDataConnection(int nCmd, int iBufSize, ActiveParams p) {

		(*mDataSock)[p.trans_id] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if ((*mDataSock)[p.trans_id] != INVALID_SOCKET
				&& !setsockopt((*mDataSock)[p.trans_id], SOL_SOCKET,
						(nCmd == CMD_STOR) ? SO_RCVBUF : SO_SNDBUF,
						(char*) &iBufSize, sizeof(int))) {

			struct sockaddr_in BindSin;
			BindSin.sin_family = AF_INET;
			BindSin.sin_addr.s_addr = p.server_ip;
#ifdef USE_BSDSOCKETS
			BindSin.sin_len = sizeof( BindSin );
#endif
			BindSin.sin_port = p.server_port;

			int on = 1; // Here the strange behaviour of SO_REUSEADDR under win32 is welcome.
#ifdef SO_REUSEPORT
			(void) setsockopt( (*mDataSock)[p.trans_id], SOL_SOCKET, SO_REUSEPORT, (char *) &on, sizeof on);
#else
			(void) setsockopt((*mDataSock)[p.trans_id], SOL_SOCKET,
					SO_REUSEADDR, (char *) &on, sizeof on);
#endif

			if (bind((*mDataSock)[p.trans_id], (struct sockaddr *) &BindSin,
					sizeof(struct sockaddr_in)) != SOCKET_ERROR) {

				struct sockaddr_in ConnectSin;
				ConnectSin.sin_family = AF_INET;
				ConnectSin.sin_port = htons( p.client_port );
				ConnectSin.sin_addr.s_addr = p.client_ip;
#ifdef USE_BSDSOCKETS
				ConnectSin.sin_len = sizeof( ConnectSin );
#endif

				if (connect((*mDataSock)[p.trans_id],
						(struct sockaddr *) &ConnectSin,
						sizeof(ConnectSin)) != SOCKET_ERROR)
					return true;

			} // else Internal Error

		}

		ResetDataConnection(p.trans_id);
		return false;
	}

	void ResetDataConnection(long long trans_id) {

		if ((*mDataSock)[trans_id] != INVALID_SOCKET) {
			SOCKET tmpSock = (*mDataSock)[trans_id];
			(*mDataSock)[trans_id] = INVALID_SOCKET;
			CloseSocket( tmpSock);
		}
	}
}
;

#endif /* SLAVESERVICESHANDER_HPP_ */
