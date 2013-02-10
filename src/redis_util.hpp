/*
 * redis_util.h
 *
 *  Created on: Sep 14, 2012
 *      Author: root
 */

#ifndef REDIS_UTIL_H_
#define REDIS_UTIL_H_
#include "hiredis/hiredis.h"
#include <string>
#include <map>
#include <set>
#include <cstdlib>

class redis_util {
public:

	template<class T> static std::map<std::string, T> to_map(redisReply* reply) {

		//build map of fields
		std::map<std::string, T> row;
		if (reply->type == REDIS_REPLY_ARRAY) {
			unsigned int i = 0;
			while (i < reply->elements) {
				row[(std::string) reply->element[i]->str] = (T)(reply->element[i + 1]->str);
				i = i + 2;
			}
		}
		freeReplyObject(reply);
		return row;
	}

	template<class T> static std::set<T> to_set(redisReply* reply) {
		std::set<std::string> set;
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

#endif /* REDIS_UTIL_H_ */
