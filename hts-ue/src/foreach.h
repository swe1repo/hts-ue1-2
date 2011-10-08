/*
 * foreach.h
 *
 *  Created on: Oct 1, 2011
 *      Author: patrick
 */

#ifndef FOREACH_H_
#define FOREACH_H_

#include <boost/foreach.hpp>

// Bypassing eclipse parser bug
#ifdef __CDT_PARSER__
	#define foreach(a, b) for(a : b)
#else
	#define foreach(a, b) BOOST_FOREACH(a, b)
#endif

#endif /* FOREACH_H_ */
