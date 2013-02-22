/**
 * Autogenerated by Thrift Compiler (0.9.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef slave_services_TYPES_H
#define slave_services_TYPES_H

#include <thrift/Thrift.h>
#include <thrift/TApplicationException.h>
#include <thrift/protocol/TProtocol.h>
#include <thrift/transport/TTransport.h>



namespace slave {

typedef struct _ActiveParams__isset {
  _ActiveParams__isset() : client_ip(false), client_port(false), fid(false), restart_at(false), transfer_buffer_size(false), server_ip(false), server_port(false) {}
  bool client_ip;
  bool client_port;
  bool fid;
  bool restart_at;
  bool transfer_buffer_size;
  bool server_ip;
  bool server_port;
} _ActiveParams__isset;

class ActiveParams {
 public:

  static const char* ascii_fingerprint; // = "BA8D7879AC362682D76912DE59F62E73";
  static const uint8_t binary_fingerprint[16]; // = {0xBA,0x8D,0x78,0x79,0xAC,0x36,0x26,0x82,0xD7,0x69,0x12,0xDE,0x59,0xF6,0x2E,0x73};

  ActiveParams() : client_ip(0), client_port(0), fid(0), restart_at(0), transfer_buffer_size(0), server_ip(0), server_port(0) {
  }

  virtual ~ActiveParams() throw() {}

  int64_t client_ip;
  int32_t client_port;
  int64_t fid;
  int64_t restart_at;
  int32_t transfer_buffer_size;
  int64_t server_ip;
  int32_t server_port;

  _ActiveParams__isset __isset;

  void __set_client_ip(const int64_t val) {
    client_ip = val;
  }

  void __set_client_port(const int32_t val) {
    client_port = val;
  }

  void __set_fid(const int64_t val) {
    fid = val;
  }

  void __set_restart_at(const int64_t val) {
    restart_at = val;
  }

  void __set_transfer_buffer_size(const int32_t val) {
    transfer_buffer_size = val;
  }

  void __set_server_ip(const int64_t val) {
    server_ip = val;
  }

  void __set_server_port(const int32_t val) {
    server_port = val;
  }

  bool operator == (const ActiveParams & rhs) const
  {
    if (!(client_ip == rhs.client_ip))
      return false;
    if (!(client_port == rhs.client_port))
      return false;
    if (!(fid == rhs.fid))
      return false;
    if (!(restart_at == rhs.restart_at))
      return false;
    if (!(transfer_buffer_size == rhs.transfer_buffer_size))
      return false;
    if (!(server_ip == rhs.server_ip))
      return false;
    if (!(server_port == rhs.server_port))
      return false;
    return true;
  }
  bool operator != (const ActiveParams &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const ActiveParams & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

void swap(ActiveParams &a, ActiveParams &b);

typedef struct _PasvParams__isset {
  _PasvParams__isset() : server_ip(false), server_port(false), fid(false), restart_at(false), transfer_buffer_size(false) {}
  bool server_ip;
  bool server_port;
  bool fid;
  bool restart_at;
  bool transfer_buffer_size;
} _PasvParams__isset;

class PasvParams {
 public:

  static const char* ascii_fingerprint; // = "CA9E086295400ABB4921368555BAB8D5";
  static const uint8_t binary_fingerprint[16]; // = {0xCA,0x9E,0x08,0x62,0x95,0x40,0x0A,0xBB,0x49,0x21,0x36,0x85,0x55,0xBA,0xB8,0xD5};

  PasvParams() : server_ip(0), server_port(0), fid(0), restart_at(0), transfer_buffer_size(0) {
  }

  virtual ~PasvParams() throw() {}

  int64_t server_ip;
  int32_t server_port;
  int64_t fid;
  int64_t restart_at;
  int32_t transfer_buffer_size;

  _PasvParams__isset __isset;

  void __set_server_ip(const int64_t val) {
    server_ip = val;
  }

  void __set_server_port(const int32_t val) {
    server_port = val;
  }

  void __set_fid(const int64_t val) {
    fid = val;
  }

  void __set_restart_at(const int64_t val) {
    restart_at = val;
  }

  void __set_transfer_buffer_size(const int32_t val) {
    transfer_buffer_size = val;
  }

  bool operator == (const PasvParams & rhs) const
  {
    if (!(server_ip == rhs.server_ip))
      return false;
    if (!(server_port == rhs.server_port))
      return false;
    if (!(fid == rhs.fid))
      return false;
    if (!(restart_at == rhs.restart_at))
      return false;
    if (!(transfer_buffer_size == rhs.transfer_buffer_size))
      return false;
    return true;
  }
  bool operator != (const PasvParams &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const PasvParams & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

void swap(PasvParams &a, PasvParams &b);

typedef struct _StorRetVal__isset {
  _StorRetVal__isset() : msg(false), size(false) {}
  bool msg;
  bool size;
} _StorRetVal__isset;

class StorRetVal {
 public:

  static const char* ascii_fingerprint; // = "1CCCF6FC31CFD1D61BBBB1BAF3590620";
  static const uint8_t binary_fingerprint[16]; // = {0x1C,0xCC,0xF6,0xFC,0x31,0xCF,0xD1,0xD6,0x1B,0xBB,0xB1,0xBA,0xF3,0x59,0x06,0x20};

  StorRetVal() : msg(), size(0) {
  }

  virtual ~StorRetVal() throw() {}

  std::string msg;
  int64_t size;

  _StorRetVal__isset __isset;

  void __set_msg(const std::string& val) {
    msg = val;
  }

  void __set_size(const int64_t val) {
    size = val;
  }

  bool operator == (const StorRetVal & rhs) const
  {
    if (!(msg == rhs.msg))
      return false;
    if (!(size == rhs.size))
      return false;
    return true;
  }
  bool operator != (const StorRetVal &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const StorRetVal & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

void swap(StorRetVal &a, StorRetVal &b);

} // namespace

#endif
