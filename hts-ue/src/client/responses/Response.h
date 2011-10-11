/*
 * Response.h
 *
 *  Created on: Oct 10, 2011
 *      Author: patrick
 */

#ifndef RESPONSE_H_
#define RESPONSE_H_

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include "../../Logging.h"
#include "../../messages/ConversionException.h"

class Response
{
public:
	enum ResponseType
	{
		ResponseTypeOk = 0,
		ResponseTypeErr,
		ResponseTypeList,
		ResponseTypeRead
	};

	Response(ResponseType type) :
		type_(type)
	{
	}

	ResponseType getType()
	{
		return type_;
	}

private:
	ResponseType type_;
};

#endif /* RESPONSE_H_ */
