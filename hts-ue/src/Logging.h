/*
 * Logging.h
 *
 *  Created on: Sep 27, 2011
 *      Author: patrick
 */

#ifndef LOGGING_H_
#define LOGGING_H_

#ifdef VERBOSE
	#include <iostream>
	#define DEBUG(s) std::cerr << __FILE__ << "::" << __LINE__ << "::" << __FUNCTION__ << "\t" << s << std::endl
#else
	#define DEBUG(s) ((void)0)
#endif

#endif /* LOGGING_H_ */
