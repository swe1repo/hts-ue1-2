/*
 * Message.cpp
 *
 *  Created on: Sep 29, 2011
 *      Author: patrick
 */

#include "Message.h"

Message::Message()
{
}


Message::~Message()
{
}

Message::MessageType Message::getType() const
{
	return Message::MessageTypeInvalid;
}

void Message::inflate(const std::string& data)
{
	throw ConversionException("Unimplemented.");
}

boost::shared_ptr<std::string> Message::deflate() const
{
	throw ConversionException("Unimplemented.");
}
