/*
 * SendMessage.h
 *
 *  Created on: Sep 29, 2011
 *      Author: patrick
 */

#ifndef SENDMESSAGE_H_
#define SENDMESSAGE_H_

#include "Message.h"
#include "Attachment.h"

// END OF TEXT CONSTANT
// used to determine the end of receivers list
#define EOT_STRING {(char)0x3, '\0'}

class SendMessage: public Message
{
public:
	SendMessage();
	SendMessage(const std::string& data);

	virtual MessageType getType() const;
	virtual void inflate(const std::string& data);
	virtual boost::shared_ptr<std::string> deflate() const;

	std::string sender_;
	std::vector<std::string> receivers_;
	std::string title_;
	std::string body_;

	std::vector<Attachment> attachments_;
};

#endif /* SENDMESSAGE_H_ */
