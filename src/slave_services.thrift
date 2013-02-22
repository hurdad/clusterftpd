namespace cpp slave

struct ActiveParams {
  1: i64 client_ip,
  2: i32 client_port,
  3: i64 fid,
  4: i64 restart_at,
  5: i32 transfer_buffer_size,
  6: i64 server_ip,
  7: i32 server_port
}

struct PasvParams {
  1: i64 server_ip, 
  2: i32 server_port
  3: i64 fid,
  4: i64 restart_at,
  5: i32 transfer_buffer_size
}

struct StorRetVal{
  1: string msg,
  2: i64 size
}

service slave_services{
	StorRetVal ActiveStoreTransfer(1:ActiveParams p, 2:i32 iflags),
	StorRetVal PasvStoreTransfer(1:PasvParams p, 2:i32 iflags),
	string ActiveRetrieveTransfer(1:ActiveParams p),
	string PasvRetrieveTransfer(1:PasvParams p)
}
