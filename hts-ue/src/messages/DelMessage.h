/*
 * DelMessage.h
 *
 *  Created on: Sep 29, 2011
 *      Author: patrick
 */

#ifndef DELMESSAGE_H_
#define DELMESSAGE_H_

#include "Message.h"

class DelMessage: public Message
{
public:
	DelMessage();
	DelMessage(const std::string& data);

	virtual MessageType getType() const;
	void inflate(const std::string& data);
	virtual boost::shared_ptr<std::string> deflate() const;

	std::string username_;
	int message_number_;
};

#endif /* DELMESSAGE_H_ */
