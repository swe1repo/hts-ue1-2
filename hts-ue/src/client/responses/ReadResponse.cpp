/*
 * ReadResponse.cpp
 *
 *  Created on: Oct 10, 2011
 *      Author: patrick
 */

#include "ReadResponse.h"

ReadResponse::ReadResponse() :
	Response(Response::ResponseTypeRead)
{
	// TODO Auto-generated constructor stub

}

SendMessage ReadResponse::getMessage()
{
	return data_;
}
