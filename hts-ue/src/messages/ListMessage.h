/*
 * ListMessage.h
 *
 *  Created on: Sep 29, 2011
 *      Author: patrick
 */

#ifndef LISTMESSAGE_H_
#define LISTMESSAGE_H_

#include "Message.h"

class ListMessage: public Message
{
public:
	ListMessage();
	ListMessage(const std::string& data);

	virtual void inflate(const std::string& data);
	virtual boost::shared_ptr<std::string> deflate() const;
	virtual MessageType getType();

	std::string username_;
};

#endif /* LISTMESSAGE_H_ */
