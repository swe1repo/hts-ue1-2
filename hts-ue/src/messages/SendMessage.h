/*
 * SendMessage.h
 *
 *  Created on: Sep 29, 2011
 *      Author: patrick
 */

#ifndef SENDMESSAGE_H_
#define SENDMESSAGE_H_

#include "Message.h"

class SendMessage: public Message
{
public:
	SendMessage();
	SendMessage(const std::string& data);

	virtual MessageType getType();
	virtual void inflate(const std::string& data);
	virtual boost::shared_ptr<std::string> deflate() const;

	std::string sender_;
	std::string receiver_;
	std::string title_;
	std::string body_;
};

#endif /* SENDMESSAGE_H_ */
