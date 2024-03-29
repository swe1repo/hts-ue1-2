/*
 * ReadMessage.cpp
 *
 *  Created on: Sep 29, 2011
 *      Author: patrick
 */

#include "ReadMessage.h"

ReadMessage::ReadMessage()
{
}

ReadMessage::ReadMessage(const std::string& data)
{
	inflate(data);
}

void ReadMessage::inflate(const std::string& data)
{
	std::stringstream ss(data);
	std::string line;

	std::getline(ss, line, '\n');

	if(line.compare("READ") != 0)
	{
		std::string error_text = "Inflation failed, because the supplied message data is not of the proper type.";

		DEBUG(error_text);
		throw ConversionException(error_text);
	}

	std::getline(ss, line, '\n');
	username_ = line;

	std::getline(ss, line, '\n');

	try
	{
		message_number_ = boost::lexical_cast<int>(line);
	}
	catch(boost::bad_lexical_cast& e)
	{
		std::string error_text = "Inflation failed, because the supplied message data is not of the proper format.";

		DEBUG(error_text);
		throw ConversionException(error_text);
	}

	DEBUG("username: " << username_ << ", number: " << message_number_);
}

boost::shared_ptr<std::string> ReadMessage::deflate() const
{
	boost::shared_ptr<std::string> deflated_string(new std::string);

	try
	{
		*deflated_string += "READ\n" + username_ + "\n" + boost::lexical_cast<std::string>(message_number_) + "\n";
	}
	catch(boost::bad_lexical_cast& e)
	{
		std::string error_text = "Deflation failed, because the supplied message data is not of the proper format.";

		DEBUG(error_text);
		throw ConversionException(error_text);
	}

	return deflated_string;
}

ReadMessage::MessageType ReadMessage::getType() const
{
	return Message::MessageTypeRead;
}
