/*
 * QuitMessage.cpp
 *
 *  Created on: Sep 29, 2011
 *      Author: patrick
 */

#include "QuitMessage.h"

QuitMessage::QuitMessage()
{
}

QuitMessage::QuitMessage(const std::string& data)
{
	inflate(data);
}

void QuitMessage::inflate(const std::string& data)
{
	std::stringstream ss(data);
	std::string line;

	std::getline(ss, line, '\n');

	if(line.compare("QUIT") != 0)
	{
		std::string error_text = "Inflation failed, because the supplied message data is not of the proper type.";

		DEBUG(error_text);
		throw ConversionException(error_text);
	}
}

boost::shared_ptr<std::string> QuitMessage::deflate() const
{
	boost::shared_ptr<std::string> deflated_string(new std::string);

	*deflated_string += "QUIT\n";

	return deflated_string;
}

QuitMessage::MessageType QuitMessage::getType() const
{
	return Message::MessageTypeQuit;
}
