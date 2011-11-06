/*
 * Attachment.h
 *
 *  Created on: Nov 5, 2011
 *      Author: patrick
 */

#ifndef ATTACHMENT_H_
#define ATTACHMENT_H_

#include <vector>
#include <string>

class Attachment
{
public:
	Attachment();
	std::string filename_;
	std::vector<char> data_;
};

#endif /* ATTACHMENT_H_ */
