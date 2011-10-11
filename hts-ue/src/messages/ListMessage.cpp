/*
 * ListMessage.cpp
 *
 *  Created on: Sep 29, 2011
 *      Author: patrick
 */

#include "ListMessage.h"

ListMessage::ListMessage()
{
}


ListMessage::ListMessage(const std::string& data)
{
	inflate(data);
}

void ListMessage::inflate(const std::string& data)
{
	std::stringstream ss(data);
	std::string line;

	std::getline(ss, line, '\n');

	if(line.compare("LIST") != 0)
	{
		std::string error_text = "Inflation failed, because the supplied message data is not of the proper type.";

		DEBUG(error_text);
		throw ConversionException(error_text);
	}

	std::getline(ss, line, '\n');
	username_ = line;

	DEBUG("username: " << username_);
}

boost::shared_ptr<std::string> ListMessage::deflate() const
{
	boost::shared_ptr<std::string> deflated_string(new std::string);

	*deflated_string += "LIST\n" + username_ + "\n";

	return deflated_string;
}

ListMessage::MessageType ListMessage::getType() const
{
	return Message::MessageTypeList;
}
