namespace cpp slave

service SlaveServices{
	bool ActiveStoreTransfer(1:string ip, 2:i16 port, 3:i64 fid, 4:i64 start_at),
	bool PasvStoreTransfer(1:string ip, 2:i16 port, 3:i64 fid, 4:i64 start_at),
	bool ActiveRetrieveTransfer(1:string ip, 2:i16 port, 3:i64 fid, 4:i64 start_at),
	bool PasvRetrieveTransfer(1:string ip, 2:i16 port, 3:i64 fid, 4:i64 start_at)
}
