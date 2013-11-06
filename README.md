clusterftpd
===========

Distributed FTP daemon written in C++ that uses Thrift for slave RPC and Redis for the Virtual File system.

PRET support for PASV transfers [http://www.drftpd.org/index.php/PRET_Specifications]

FTP Daemon core using CFtpServer Library[http://sourceforge.net/projects/cftpserver/support]

Architecture:

Metaserver: Single metaserver handles all ftp control channel communication. All transfer request are sent to slaves. A single redis-server database instance holds virtual file system, remote or local.

Slaveserver: [Thrift RPC Server] on each server handles transfers to/from clients.

### Gettting Started


```
git clone https://github.com/hurdad/clusterftpd.git
cd clusterftpd
./bootstrap
./configure CPPFLAGS="-DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H"
make
```
