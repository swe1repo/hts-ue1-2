/*
 * QuitMessage.h
 *
 *  Created on: Sep 29, 2011
 *      Author: patrick
 */

#ifndef QUITMESSAGE_H_
#define QUITMESSAGE_H_

#include "Message.h"

class QuitMessage: public Message
{
public:
	QuitMessage();
	QuitMessage(const std::string& data);

	virtual MessageType getType() const;
	virtual void inflate(const std::string& data);
	virtual boost::shared_ptr<std::string> deflate() const;
};

#endif /* QUITMESSAGE_H_ */
