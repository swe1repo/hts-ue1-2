/*
 * ReadResponse.h
 *
 *  Created on: Oct 10, 2011
 *      Author: patrick
 */

#ifndef READRESPONSE_H_
#define READRESPONSE_H_

#include "Response.h"
#include "../../messages/SendMessage.h"

class ReadResponse : public Response
{
public:
	ReadResponse(const SendMessage& msg);
	SendMessage getMessage();
private:
	SendMessage data_;
};

#endif /* READRESPONSE_H_ */
