/*
 * MessageIdGenerator.h
 *
 *  Created on: Nov 5, 2011
 *      Author: patrick
 */

#ifndef MESSAGEIDGENERATOR_H_
#define MESSAGEIDGENERATOR_H_

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <vector>
#include <string>
#include <boost/filesystem.hpp>
#include "Singleton.h"

class MessageIdGenerator : public Singleton<MessageIdGenerator>
{
public:
	std::string getUniqueMessageId();

private:
	MessageIdGenerator();

	friend class Singleton<MessageIdGenerator>;
};

#endif /* MESSAGEIDGENERATOR_H_ */
