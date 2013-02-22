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
#define O_BINARY 0
#define CloseSocket(x)	(shutdown((x), SHUT_RDWR), close(x))

using namespace slave;
using namespace std;

class slave_servicesHandler: virtual public slave_servicesIf {
public:
	slave_servicesHandler() {
		// Your initialization goes here
	}

	void ActiveStoreTransfer(StorRetVal& _return, const ActiveParams& p,
			const int32_t iflags) {

		if (!OpenActiveDataConnection(CMD_STOR, p.transfer_buffer_size, p)) {
			_return.msg = "425 Can't open data connection.";
			_return.size = -1;
			return;
		}

		Store(_return, p.transfer_buffer_size, p.fid, p.restart_at, iflags);
	}

	void PasvStoreTransfer(StorRetVal& _return, const PasvParams& p,
			const int32_t iflags) {

		if (!OpenPasvDataConnection(CMD_STOR, p.transfer_buffer_size, p)) {
			_return.msg = "425 Can't open data connection.";
			_return.size = -1;
			return;
		}

		Store(_return, p.transfer_buffer_size, p.fid, p.restart_at, iflags);
	}

	void ActiveRetrieveTransfer(std::string& _return, const ActiveParams& p) {

		if (!OpenActiveDataConnection(CMD_RETR, p.transfer_buffer_size, p)) {
			_return = "425 Can't open data connection.";
			return;
		}

		Retrieve(_return, p.transfer_buffer_size, p.fid, p.restart_at);
	}

	void PasvRetrieveTransfer(std::string& _return, const PasvParams& p) {

		if (!OpenPasvDataConnection(CMD_RETR, p.transfer_buffer_size, p)) {
			_return = "425 Can't open data connection.";
			return;
		}

		Retrieve(_return, p.transfer_buffer_size, p.fid, p.restart_at);
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

	volatile SOCKET DataSock;

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
			const int64_t fid, const int64_t restart_at) {

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
				while (DataSock != INVALID_SOCKET
						&& (BlockSize = read(hFile, pBuffer, uiBufferSize)) > 0) {

					{
						len = send(DataSock, pBuffer, BlockSize, MSG_NOSIGNAL);
						if (len <= 0)
							break;
					}
				}
			} // else Internal Error
			close(hFile);
		}

		endofretrieve: if (pBuffer)
			delete[] pBuffer;

		ResetDataConnection();

		if (len < 0 || hFile == -1)
			_return = "550 Can't retrieve File.";
		else
			_return = "226 Transfer complete.";

	}

	void Store(StorRetVal& _return, const int32_t transfer_buffer_size,
			const int64_t fid, const int64_t restart_at, const int32_t iflags) {

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
		endofstore:

		if (pBuffer)
			delete[] pBuffer;

		ResetDataConnection();

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

	bool OpenPasvDataConnection(int nCmd, int iBufSize, PasvParams p) {

		DataSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		struct sockaddr_in sin;
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = p.server_ip;
#ifdef USE_BSDSOCKETS
		sin.sin_len = sizeof( sin );
#endif
		int on = 1;
		if (DataSock != INVALID_SOCKET
				&& setsockopt(DataSock, SOL_SOCKET, SO_REUSEADDR, (char *) &on,
						sizeof(on)) != SOCKET_ERROR) {

			sin.sin_port = htons( p.server_port );

			if (bind(DataSock, (struct sockaddr *) &sin,
					sizeof(struct sockaddr_in))) {

				if (listen(DataSock, 1) != SOCKET_ERROR) {

					SOCKET sTmpWs;
					struct sockaddr_in sin;
					socklen_t sin_len = sizeof(struct sockaddr_in);
#ifdef USE_BSDSOCKETS
					sin.sin_len = sizeof( sin );
#endif

					fd_set fdRead;
					FD_ZERO( &fdRead);
					FD_SET( DataSock, &fdRead);
					timeval tv;
					tv.tv_sec = 10;
					tv.tv_usec = 0;

					if (select(DataSock + 1, &fdRead, NULL, NULL, &tv)
							> 0&& FD_ISSET( DataSock, &fdRead )) {
						sTmpWs = accept(DataSock, (struct sockaddr *) &sin,
								&sin_len);
						CloseSocket( DataSock);
						DataSock = sTmpWs;
						if (sTmpWs != INVALID_SOCKET) {
							//		SendReply("150 Connection accepted.");
							return true;
						}
					}
				}
			}
		}

		ResetDataConnection();
		return false;

	}
	bool OpenActiveDataConnection(int nCmd, int iBufSize, ActiveParams p) {

		DataSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (DataSock != INVALID_SOCKET
				&& !setsockopt(DataSock, SOL_SOCKET,
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
			(void) setsockopt( DataSock, SOL_SOCKET, SO_REUSEPORT, (char *) &on, sizeof on);
#else
			(void) setsockopt(DataSock, SOL_SOCKET, SO_REUSEADDR, (char *) &on,
					sizeof on);
#endif

			if (bind(DataSock, (struct sockaddr *) &BindSin,
					sizeof(struct sockaddr_in)) != SOCKET_ERROR) {

				struct sockaddr_in ConnectSin;
				ConnectSin.sin_family = AF_INET;
				ConnectSin.sin_port = htons( p.client_port );
				ConnectSin.sin_addr.s_addr = p.client_ip;
#ifdef USE_BSDSOCKETS
				ConnectSin.sin_len = sizeof( ConnectSin );
#endif

				if (connect(DataSock, (struct sockaddr *) &ConnectSin,
						sizeof(ConnectSin)) != SOCKET_ERROR)
					return true;

			} // else Internal Error

		}

		ResetDataConnection();
		return false;
	}

	void ResetDataConnection() {

		if (DataSock != INVALID_SOCKET) {
			SOCKET tmpSock = DataSock;
			DataSock = INVALID_SOCKET;
			CloseSocket( tmpSock);
		}
	}
}
;

#endif /* SLAVESERVICESHANDER_HPP_ */
