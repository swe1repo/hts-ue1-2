/*
 * MessageFactory.cpp
 *
 *  Created on: Oct 1, 2011
 *      Author: patrick
 */

#include "MessageFactory.h"

MessageFactory::MessageFactory()
{
}

Message* MessageFactory::newInstance(const std::vector< boost::shared_ptr<std::string> >& data)
{
	if(data.size() < 1)
	{
		std::string text("MessageFactory: No data has been provided for construction.");
		DEBUG(text);
		throw ConversionException(text);
	}

	Message::MessageType type = getMessageType( *data.at(0) );

	auto pstring_data = dataToString(data);

	switch(type)
	{
	case Message::MessageTypeSend:
		return new SendMessage(*pstring_data);
	case Message::MessageTypeList:
		return new ListMessage(*pstring_data);
	case Message::MessageTypeRead:
		return new ReadMessage(*pstring_data);
	case Message::MessageTypeDel:
		return new DelMessage(*pstring_data);
	case Message::MessageTypeQuit:
		return new QuitMessage(*pstring_data);
	case Message::MessageTypeLogin:
		return new LoginMessage(*pstring_data);
	case Message::MessageTypeInvalid:
	default:
		std::string text("MessageFactory: Unknown type can't be instantiated.");
		throw ConversionException(text);
	}
}

Message::MessageType MessageFactory::getMessageType(std::string line)
{
	if(line.compare("SEND") == 0)
	{
		return Message::MessageTypeSend;
	}
	else if(line.compare("LIST") == 0)
	{
		return Message::MessageTypeList;
	}
	else if(line.compare("READ") == 0)
	{
		return Message::MessageTypeRead;
	}
	else if(line.compare("DEL") == 0)
	{
		return Message::MessageTypeDel;
	}
	else if(line.compare("QUIT") == 0)
	{
		return Message::MessageTypeQuit;
	}
	else if(line.compare("LOGIN") == 0)
	{
		return Message::MessageTypeLogin;
	}
	else
	{
		DEBUG("MessageFactory: Message type " << line << " is unknown.");
		return Message::MessageTypeInvalid;
	}
}

boost::shared_ptr<std::string> MessageFactory::dataToString(const std::vector< boost::shared_ptr<std::string> >& data)
{
	boost::shared_ptr<std::string> retVal(new std::string());

	foreach(boost::shared_ptr<std::string> str, data)
	{
		retVal->append(*str + std::string("\n"));
	}

	return retVal;
}
