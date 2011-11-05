/*
 * FileLockDelegate.h
 *
 *  Created on: Nov 4, 2011
 *      Author: patrick
 */

#ifndef FILELOCKDELEGATE_H_
#define FILELOCKDELEGATE_H_

#include <string>

class FileLockDelegate
{
public:
	void lock(std::string file_path);
	void unlock(std::string file_path);
};

#endif /* FILELOCKDELEGATE_H_ */
