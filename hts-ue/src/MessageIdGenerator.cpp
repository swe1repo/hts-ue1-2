/*
 * MessageIdGenerator.cpp
 *
 *  Created on: Nov 5, 2011
 *      Author: patrick
 */

#include "MessageIdGenerator.h"

namespace fs = boost::filesystem;

MessageIdGenerator::MessageIdGenerator()
{
	// use current time as seed
	srand48(time(0));
}

std::string MessageIdGenerator::getUniqueMessageId()
{
	const int FILENAME_LEN = 16;
	char buf[ FILENAME_LEN + 1];

	// repeat until we have found a unique messageId
	do
	{
		// this is the generated random values
		// 1 byte is is converted to 2 hex numbers
		// thus FILENAME_LEN / 2 bytes are needed.
		std::vector<long int> randVal;

		for(unsigned int i = 0; i < FILENAME_LEN / (sizeof(long int) * 2); ++i)
		{
			// generate random 48 bit number
			randVal.push_back( lrand48() );
		}

		size_t size = sizeof(long int) * 2;

		auto it = randVal.begin();
		for(int i = 0; it != randVal.end(); i += size, it++)
		{
			// convert to hex and then to characters
			snprintf(&buf[i], size + 1, "%0*lx", size, *it);
		}

		buf[ FILENAME_LEN ] = 0;
	}
	while( fs::exists( fs::path(buf) ) );

	return std::string(buf);
}
