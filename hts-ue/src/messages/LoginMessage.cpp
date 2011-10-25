/*
 * LoginMessage.cpp
 *
 *  Created on: Oct 18, 2011
 *      Author: patrick
 */

#include "LoginMessage.h"

LoginMessage::LoginMessage()
{
}

LoginMessage::LoginMessage(const std::string& data)
{
	inflate(data);
}

void LoginMessage::inflate(const std::string& data)
{
	std::stringstream ss(data);
	std::string line;

	std::getline(ss, line, '\n');

	if(line.compare("LOGIN") != 0)
	{
		std::string error_text = "Inflation failed, because the supplied message data is not of the proper type.";

		DEBUG(error_text);
		throw ConversionException(error_text);
	}

	std::getline(ss, line, '\n');
	username_ = line;

	std::getline(ss, line, '\n');
	password_ = line;
}

boost::shared_ptr<std::string> LoginMessage::deflate() const
{
	boost::shared_ptr<std::string> deflated_string(new std::string);

	*deflated_string += "LOGIN\n" + username_ + "\n" + password_ + "\n";

	return deflated_string;
}

Message::MessageType LoginMessage::getType() const
{
	return Message::MessageTypeLogin;
}
