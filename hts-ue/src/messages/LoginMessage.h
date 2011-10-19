/*
 * LoginMessage.h
 *
 *  Created on: Oct 18, 2011
 *      Author: patrick
 */

#ifndef LOGINMESSAGE_H_
#define LOGINMESSAGE_H_

#include "Message.h"

class LoginMessage: public Message
{
public:
	LoginMessage();
	LoginMessage(const std::string& data);

	virtual void inflate(const std::string& data);
	virtual boost::shared_ptr<std::string> deflate() const;

	std::string username_;
	std::string password_;
};

#endif /* LOGINMESSAGE_H_ */
