#include <pthread.h>
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PosixThreadFactory.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include <iostream>
#include <stdexcept>
#include <sstream>

#include "slaveserver_main.h"
#include "slaveserver_log.hpp"
#include "slaveserver_config.hpp"
#include "slave_servicesHandler.hpp"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;
using namespace apache::thrift::concurrency;

using namespace boost;

int main(int argc, char * argv[]) {

	if(argc != 2){
		cout << "Usage: slaveserver slaveserver-sample.cfg" << endl;
		//log error
		//Log::OnServerEvent(
		return 1;
	}

	//init config
	slave_server::InitServerConfig();
	slave_server::LoadServerConfig(argv[1]);

	shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
	shared_ptr<slave_servicesHandler> handler(new slave_servicesHandler());
	shared_ptr<TProcessor> processor(new slave_servicesProcessor(handler));
	shared_ptr<TServerTransport> serverTransport(
			new TServerSocket(slave_server::server.ThriftPort));
	shared_ptr<TTransportFactory> transportFactory(
			new TBufferedTransportFactory());

	shared_ptr<ThreadManager> threadManager =
			ThreadManager::newSimpleThreadManager(slave_server::server.WorkerCount);
	shared_ptr<PosixThreadFactory> threadFactory =
			shared_ptr<PosixThreadFactory>(new PosixThreadFactory());
	threadManager->threadFactory(threadFactory);
	threadManager->start();
	TThreadPoolServer server(processor, serverTransport, transportFactory,
			protocolFactory, threadManager);

	printf("Starting the server...\n");
	server.serve();
	printf("done.\n");

	return 0;
}
