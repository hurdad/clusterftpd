namespace cpp slave

struct Params {
  1: string client_ip,
  2: i32 client_port,
  3: i64 fid,
  4: i64 restart_at,
  5: i32 transfer_buffer_size,
  6: i64 server_ip,
  7: i32 data_range_start,
  8: i32 data_range_len
}

service slave_services{
	bool ActiveStoreTransfer(1:Params p, 2:i32 iflags),
	bool PasvStoreTransfer(1:Params p, 2:i32 iflags),
	bool ActiveRetrieveTransfer(1:Params p),
	bool PasvRetrieveTransfer(1:Params p)
}
