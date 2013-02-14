/*
 * SlaveServicesHander.hpp
 *
 *  Created on: Feb 14, 2013
 *      Author: alex
 */

#ifndef SLAVESERVICESHANDER_HPP_
#define SLAVESERVICESHANDER_HPP_

#include "../gen-cpp/SlaveServices.h"
using namespace slave;

class SlaveServicesHandler : virtual public SlaveServicesIf {
 public:
  SlaveServicesHandler() {
    // Your initialization goes here
  }

  bool ActiveStoreTransfer(const std::string& ip, const int16_t port, const int64_t fid, const int64_t start_at) {
    // Your implementation goes here
    printf("ActiveStoreTransfer\n");
  }

  bool PasvStoreTransfer(const std::string& ip, const int16_t port, const int64_t fid, const int64_t start_at) {
    // Your implementation goes here
    printf("PasvStoreTransfer\n");
  }

  bool ActiveRetrieveTransfer(const std::string& ip, const int16_t port, const int64_t fid, const int64_t start_at) {
    // Your implementation goes here
    printf("ActiveRetrieveTransfer\n");
  }

  bool PasvRetrieveTransfer(const std::string& ip, const int16_t port, const int64_t fid, const int64_t start_at) {
    // Your implementation goes here
    printf("PasvRetrieveTransfer\n");
  }

};

#endif /* SLAVESERVICESHANDER_HPP_ */
