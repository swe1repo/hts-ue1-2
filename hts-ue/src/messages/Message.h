/*
 * Message.h
 *
 *  Created on: Sep 29, 2011
 *      Author: patrick
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <string>
#include <iostream>
#include <sstream>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include "../Logging.h"
#include "ConversionException.h"

class Message
{
public:
	enum MessageType
	{
		MessageTypeSend = 0,
		MessageTypeList,
		MessageTypeRead,
		MessageTypeDel,
		MessageTypeQuit,
		MessageTypeLogin,
		MessageTypeInvalid
	};

	Message();
	virtual ~Message();

	virtual MessageType getType() const;
	virtual void inflate(const std::string& data);
	virtual boost::shared_ptr<std::string> deflate() const;
};

#endif /* MESSAGE_H_ */
