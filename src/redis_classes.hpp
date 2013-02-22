/*
 * redis_vfs.h
 *
 *  Created on: Sep 14, 2012
 *      Author: root
 */

#ifndef REDIS_VFS_H_
#define REDIS_VFS_H_
#include "hiredis/hiredis.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <cstdlib>

using namespace std;
typedef map<string, long long> VDIR;

class redis_util {
public:
	template<class T> static map<string, T> to_map(redisReply* reply) {

		//build map of fields
		map<string, T> row;
		if (reply->type == REDIS_REPLY_ARRAY) {
			unsigned int i = 0;
			while (i < reply->elements) {
				row[(string) reply->element[i]->str] = boost::lexical_cast<T>(
						reply->element[i + 1]->str);
				i = i + 2;
			}
		}
		freeReplyObject(reply);
		return row;
	}

	template<class T> static set<T> to_set(redisReply* reply) {
		set<string> set;
		if (reply->type == REDIS_REPLY_ARRAY) {
			unsigned int i = 0;
			while (i < reply->elements) {
				set.insert(boost::lexical_cast<T>(reply->element[i]->str));
				i++;
			}
		}
		freeReplyObject(reply);
		return set;
	}
};

struct slave_info {
	string host;
	unsigned short port;
};

class redis_vfs {
public:

	static VDIR* opendir(redisContext *c, const char *path) {

		long long fid = lookup_fid(c, path);
		if (fid < 0)
			return NULL;

		//query folders
		redisReply* reply = (redisReply*) redisCommand(c, "HGETALL dirent:%u",
				fid);

		if ( reply->elements > 0) {
			return new map<string, long long>(
					redis_util::to_map<long long>(reply));
		}

		return NULL;

	}

	dirent static *readdir(VDIR *dp) {

		dirent *my_dirent = (dirent *) malloc(sizeof(dirent));
		if (!my_dirent)
			return NULL;

		//iterator
		VDIR::iterator it;

		//check size
		if (dp->size() > 0) {
			it = dp->begin();
			strcpy(my_dirent->d_name, (*it).first.c_str());
			my_dirent->d_ino = (*it).second;
			dp->erase(it);
			return my_dirent;
		} else {
			delete dp;
		}

		return NULL;

	}

	int static stat(redisContext *c, const char *path, struct stat *buf) {

		long long fid = lookup_fid(c, path);
		if (fid < 0)
			return -1;

		redisReply* reply = (redisReply*) redisCommand(c, "HGETALL stat:%u",
				fid);

		if (reply->elements > 0) {
			map<string, string> stat = redis_util::to_map<string>(reply);

			buf->st_mode = atoi(stat["st_mode"].c_str());
			buf->st_size = atoi(stat["st_size"].c_str());
			buf->st_uid = atoi(stat["st_uid"].c_str());
			buf->st_gid = atoi(stat["st_gid"].c_str());
			buf->st_mtime = atoi(stat["st_mtime"].c_str());

			return 0;
		}
		return -1;
	}

	int static mkdir(redisContext *c, const char *path, mode_t mode,
			__uid_t uid, __gid_t gid) {

		long long new_fid = get_new_fid(c);

		//TODO convert mode Octal to decimal assuming 0777
		mode = 16895;

		//diren entry
		string spath(path);
		unsigned found = spath.find_last_of("/\\");
		string parentpath = spath.substr(0, found + 1);
		long long parent_fid = lookup_fid(c, parentpath.c_str());
		if (parent_fid < 0) {
			return -1;
		}
		string entry = spath.substr(found + 1);

		//redis update
		redisCommand(c, "MULTI");

		//stat entry
		redisCommand(c, "HSET stat:%u st_size %u", new_fid, 4096);
		redisCommand(c, "HSET stat:%u st_mode %u", new_fid, mode);
		redisCommand(c, "HSET stat:%u st_uid %u", new_fid, uid);
		redisCommand(c, "HSET stat:%u st_gid %u", new_fid, gid);
		redisCommand(c, "HSET stat:%u st_mtime %u", new_fid, time(NULL));

		//parent dirent
		redisCommand(c, "HSET dirent:%u %s %u", parent_fid, entry.c_str(),
				new_fid);

		//new dirents
		redisCommand(c, "HSET dirent:%u .foo -1", new_fid);

		//commit
		redisCommand(c, "EXEC");

		//check for errors
		if (c != NULL && c->err) {
			return -1;
		}

		return 0;
	}

	int static rmdir(redisContext *c, const char *path) {

		long long fid = lookup_fid(c, path);
		if (fid < 0)
			return -1;

		//check for empty dirent
		redisReply* reply = (redisReply*) redisCommand(c, "HLEN dirent:%u",
				fid);

		long long size = reply->integer;
		freeReplyObject(reply);
		if (size == 0) {

			//parents dirent
			string spath(path);
			unsigned found = spath.find_last_of("/\\");
			string parentpath = spath.substr(0, found + 1);
			long long parent_fid = lookup_fid(c, parentpath.c_str());
			if (parent_fid < 0) {
				return -1;
			}
			string entry = spath.substr(found + 1);

			//redis update
			redisCommand(c, "MULTI");
			redisCommand(c, "HDEL dirent:%u %s", parent_fid, entry.c_str());
			redisCommand(c, "DEL stat:%u", fid);
			redisCommand(c, "DEL diren:%u ", fid);
			redisCommand(c, "EXEC");

			//check for errors
			if (c != NULL && c->err) {
				return -1;
			}

			return 0;
		}
		return -1;
	}

	int static rename(redisContext *c, const char *oldpath,
			const char *newpath) {

		long long fid = lookup_fid(c, oldpath);
		if (fid < 0)
			return -1;

		//remove old entry
		string soldpath(oldpath);
		unsigned found = soldpath.find_last_of("/\\");
		string oldparentpath = soldpath.substr(0, found + 1);
		long long oldparent_fid = lookup_fid(c, oldparentpath.c_str());
		if (oldparent_fid < 0) {
			return -1;
		}
		string oldentry = soldpath.substr(found + 1);

		//add new entry
		string snewpath(newpath);
		found = snewpath.find_last_of("/\\");
		string newparentpath = snewpath.substr(0, found + 1);
		long long newparent_fid = lookup_fid(c, oldparentpath.c_str());
		if (newparent_fid < 0) {
			return -1;
		}
		string newentry = snewpath.substr(found + 1);

		//redis update
		redisCommand(c, "MULTI");
		redisCommand(c, "HDEL dirent:%u %s", oldparent_fid, oldentry.c_str());
		redisCommand(c, "HSET dirent:%u %s %u ", newparent_fid,
				newentry.c_str(), fid);
		redisCommand(c, "EXEC");

		//check for errors
		if (c != NULL && c->err) {
			return -1;
		}

		return 0;
	}

	int static remove(redisContext *c, const char *path) {

		long long fid = lookup_fid(c, path);
		if (fid < 0)
			return -1;

		//parents dirent
		string spath(path);
		unsigned found = spath.find_last_of("/\\");
		string parentpath = spath.substr(0, found + 1);
		long long parent_fid = lookup_fid(c, parentpath.c_str());
		if (parent_fid < 0) {
			return -1;
		}
		string entry = spath.substr(found + 1);

		//update redis
		redisCommand(c, "MULTI");
		redisCommand(c, "HDEL dirent:%u %s", parent_fid, entry.c_str());
		redisCommand(c, "DEL stat:%u", fid);

		//commit
		redisCommand(c, "EXEC");

		//check for errors
		if (c != NULL && c->err) {
			return -1;
		}

		return 0;
	}

	long long static get_new_fid(redisContext *c) {
		redisReply* reply = (redisReply*) redisCommand(c, "INCR fid_counter");
		freeReplyObject(reply);
		return reply->integer;
	}

	int static save_new_file(redisContext *c, const char *path, mode_t mode,
			long long size, __uid_t uid, __gid_t gid, long long fid,
			slave_info slave_ip_port) {

		//TODO convert mode Octal to decimal assuming 0777
		mode = 33279;

		//diren entry
		string spath(path);
		unsigned found = spath.find_last_of("/\\");
		string parentpath = spath.substr(0, found + 1);
		long long parent_fid = lookup_fid(c, parentpath.c_str());
		if (parent_fid < 0) {
			return -1;
		}
		string entry = spath.substr(found + 1);

		//update redis
		redisCommand(c, "MULTI");
		redisCommand(c, "HSET dirent:%u %s %u", parent_fid, entry.c_str(), fid);

		//stat entry
		redisCommand(c, "HSET stat:%u st_mode %u", fid, mode);
		redisCommand(c, "HSET stat:%u st_size %u", fid, size);
		redisCommand(c, "HSET stat:%u st_uid %u", fid, uid);
		redisCommand(c, "HSET stat:%u st_gid %u", fid, gid);
		redisCommand(c, "HSET stat:%u st_mtime %u", fid, time(NULL));
		redisCommand(c, "HSET stat:%u slave_host %s", fid,
				slave_ip_port.host.c_str());
		redisCommand(c, "HSET stat:%u slave_port %u", fid, slave_ip_port.port);
		//commit
		redisCommand(c, "EXEC");

		//check for errors
		if (c != NULL && c->err) {
			return -1;
		}

		return 0;
	}

	static slave_info lookup_slave_info(redisContext *c, const char *path) {
		//assume fid exist
		long long fid = lookup_fid(c, path);

		//init return var
		slave_info info;

		redisReply* reply = (redisReply*) redisCommand(c, "HGETALL stat:%u",
				fid);

		if (reply->elements > 0) {
			map<string, string> stat = redis_util::to_map<string>(reply);

			info.host = stat["slave_host"];
			info.port = atoi(stat["slave_port"].c_str());
		}

		return info;
	}

	long long static lookup_fid(redisContext *c, const char *path) {

		vector<string> tokens;
		boost::split(tokens, path, boost::is_any_of("/"));

		//root
		if (tokens.size() == 2 && tokens[1] == "")
			return 0;

		//init root seed
		int fid = 0;

		//remove first '/' space token
		tokens.erase(tokens.begin());

		//remove trailing '/' space token, if exist
		if (tokens.back() == "")
			tokens.erase(tokens.end());

		while (tokens.size() > 0) {
			redisReply* reply = (redisReply*) redisCommand(c,
					"HGETALL dirent:%u", fid);

			if (reply->elements > 0) {
				map<string, long long> my_map = map<string, long long>(
						redis_util::to_map<long long>(reply));

				if (my_map.find(tokens.front()) != my_map.end()) {
					fid = my_map[tokens.front()];
					tokens.erase(tokens.begin());
				} else {
					//not found
					return -1;
				}
			}
		}

		return fid;
	}

};

#endif /* REDIS_VFS_H_ */
