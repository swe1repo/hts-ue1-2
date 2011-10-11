/*
 * ReadMessage.h
 *
 *  Created on: Sep 29, 2011
 *      Author: patrick
 */

#ifndef READMESSAGE_H_
#define READMESSAGE_H_

#include "Message.h"

class ReadMessage: public Message
{
public:
	ReadMessage();
	ReadMessage(const std::string& data);

	virtual MessageType getType() const;
	virtual void inflate(const std::string& data);
	virtual boost::shared_ptr<std::string> deflate() const;

	std::string username_;
	int message_number_;
};

#endif /* READMESSAGE_H_ */
