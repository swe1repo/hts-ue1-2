/*
 * ReadResponse.cpp
 *
 *  Created on: Oct 10, 2011
 *      Author: patrick
 */

#include "ReadResponse.h"

ReadResponse::ReadResponse(const SendMessage& msg) :
	Response(Response::ResponseTypeRead),
	data_(msg)
{
}

SendMessage ReadResponse::getMessage()
{
	return data_;
}
