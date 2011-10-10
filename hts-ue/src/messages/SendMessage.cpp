/*
 * SendMessage.cpp
 *
 *  Created on: Sep 29, 2011
 *      Author: patrick
 */

#include "SendMessage.h"

SendMessage::SendMessage()
{
}

SendMessage::SendMessage(const std::string& data)
{
	inflate(data);
}

void SendMessage::inflate(const std::string& data)
{
	std::stringstream ss(data);
	std::string line;

	std::getline(ss, line, '\n');

	if(line.compare("SEND") != 0)
	{
		std::string error_text = "Inflation failed, because the supplied message data is not of the proper type.";

		DEBUG(error_text);
		throw ConversionException(error_text);
	}

	std::getline(ss, line, '\n');
	sender_ = line;

	std::getline(ss, line, '\n');
	receiver_ = line;

	std::getline(ss, line, '\n');
	title_ = line;

	while(true)
	{
		std::getline(ss, line, '\n');

		if(line.compare(".") == 0)
		{
			// message body ended
			break;
		}
		else
		{
			// re-append stripped newlines
			body_.append(line + "\n");
		}
	}

	DEBUG("sender: " << sender_ <<
	    ", receiver: " << receiver_ <<
	    ", title: " << title_ <<
	    ", body: " << body_);
}

boost::shared_ptr<std::string> SendMessage::deflate() const
{
	boost::shared_ptr<std::string> deflated_string(new std::string);

	*deflated_string += "SEND\n" + sender_ + "\n" + receiver_ + "\n" + title_ + "\n" + body_ + "\n.\n";

	return deflated_string;
}

SendMessage::MessageType SendMessage::getType()
{
	return Message::MessageTypeSend;
}
