/*
 * redis_vfs.h
 *
 *  Created on: Sep 14, 2012
 *      Author: root
 */

#ifndef REDIS_VFS_H_
#define REDIS_VFS_H_
#include "hiredis/hiredis.h"
#include <string>
#include <map>
#include <set>
#include <cstdlib>

using namespace std;
typedef std::set<std::string> VDIR;

class redis_util {
public:
	template<class T> static map<string, T> to_map(redisReply* reply) {

		//build map of fields
		map<string, T> row;
		if (reply->type == REDIS_REPLY_ARRAY) {
			unsigned int i = 0;
			while (i < reply->elements) {
				row[(string) reply->element[i]->str] =
						(T) (reply->element[i + 1]->str);
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
				set.insert((T) reply->element[i]->str);
				i++;
			}
		}
		freeReplyObject(reply);
		return set;
	}
};

class redis_vfs {
public:

	static VDIR* opendir(redisContext *c, const char *dirname) {

		//query folders
		redisReply* reply = (redisReply*) redisCommand(c, "SMEMBERS dirent:%s",
				dirname);

		//check if user exist
		if (reply->elements > 0) {
			//	set<string> redis_vfs::to_set<string>(reply);
			return new set<string>(redis_util::to_set < string > (reply));
		}

		return NULL;

	}

	int static readdir_r(VDIR *dp, struct dirent *entry,
			struct dirent **result) {

		//iterator
		VDIR::iterator it;

		//check size
		if (dp->size() > 0) {
			it = dp->begin();
			strcpy(entry->d_name, (*it).c_str());
			dp->erase(it);
			result = &entry;
			return 0;
		}

		result = NULL;
		return -1;
	}

	int static stat(redisContext *c, const char *path, struct stat *buf) {

		redisReply* reply = (redisReply*) redisCommand(c, "HGETALL stat:%s",
				path);

		if (reply->elements > 0) {
			map<string, string> stat = redis_util::to_map < string > (reply);

			buf->st_mode = atoi(stat["st_mode"].c_str());
			buf->st_size = atoi(stat["st_size"].c_str());
			buf->st_uid = atoi(stat["st_uid"].c_str());
			buf->st_gid = atoi(stat["st_gid"].c_str());
			buf->st_mtime = atoi(stat["st_mtime"].c_str());

			return 0;
		}
		return -1;
	}

	int static mkdir(redisContext *c, const char *path, mode_t mode, __uid_t uid, __gid_t gid) {

		//Add IS_DIR to mode

		redisCommand(c, "MULTI");

		//stat entry
		redisCommand(c, "HSET stat:%s st_mode %s", path, mode);
		redisCommand(c, "HSET stat:%s st_size %s", path, 4096);
		redisCommand(c, "HSET stat:%s st_uid %s", path, uid);
		redisCommand(c, "HSET stat:%s st_gid %s", path, gid);
		redisCommand(c, "HSET stat:%s st_mtime %s", path, time(NULL));
		//diren entry

		string parentpath, entry;
		redisCommand(c, "SADD dirent:%s %s", parentpath.c_str(), entry.c_str());

		redisReply* reply = (redisReply*) redisCommand(c, "EXEC");

		//TODO Check Errors

		//	if(error)
		//	return -1;

		return 0;
	}

	int static rmdir(redisContext *c, const char *path) {

		//check for empty dirent
		redisReply* reply = (redisReply*) redisCommand(c, "SCARD dirent:%s",
				path);

		if (reply->integer == 0) {

			redisCommand(c, "MULTI");
			redisCommand(c, "DEL stat:%s", path);
			redisCommand(c, "DEL diren:%s ", path);
			redisCommand(c, "EXEC");
			//TODO check for errors

			return 0;
		}
		return -1;
	}

	int static rename(redisContext *c, const char *oldpath,
			const char *newpath) {

		redisReply* reply = (redisReply*) redisCommand(c, "HGETALL stat:%s",
				oldpath);

		//get oldpath
		if (reply->elements > 0) {
			map<string, string> stat = redis_util::to_map < string > (reply);

			//set newpath
			redisCommand(c, "MULTI");

			//stat entry
			redisCommand(c, "HSET stat:%s fid %s", newpath,
					stat["fid"].c_str());
			redisCommand(c, "HSET stat:%s st_mode %s", newpath,
					stat["st_mode"].c_str());
			redisCommand(c, "HSET stat:%s st_size %s", newpath,
					stat["st_size"].c_str());
			redisCommand(c, "HSET stat:%s st_uid %s", newpath,
					stat["st_uid"].c_str());
			redisCommand(c, "HSET stat:%s st_gid %s", newpath,
					stat["st_gid"].c_str());
			redisCommand(c, "HSET stat:%s st_mtime %s", newpath,
					stat["st_mtime"].c_str());
			redisCommand(c, "HSET stat:%s slave_ip:port %s", newpath,
					stat["slave_ip:port"].c_str());

			//dirent entry

			string oldpath, oldentry, newpath, newentry;

			//TODO parse paths/entrys
			redisCommand(c, "SREM dirent:%s %s", oldpath.c_str(),
					oldentry.c_str());
			redisCommand(c, "SADD dirent:%s %s", newpath.c_str(),
					newentry.c_str());

			redisCommand(c, "EXEC");

			//TODO check for errors

			return 0;

		}

		return -1;
	}

	int static remove(redisContext *c, const char *pathname) {

		redisCommand(c, "MULTI");
		redisCommand(c, "DEL stat:%s", pathname);
		redisCommand(c, "DEL diren:%s", pathname);
		redisCommand(c, "EXEC");
		//TODO check for errors

		return 0;
	}
};

#endif /* REDIS_VFS_H_ */
