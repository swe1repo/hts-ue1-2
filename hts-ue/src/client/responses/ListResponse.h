/*
 * ListResponse.h
 *
 *  Created on: Oct 10, 2011
 *      Author: patrick
 */

#ifndef LISTRESPONSE_H_
#define LISTRESPONSE_H_

#include "Response.h"

class ListResponse : public Response
{
public:
	ListResponse(const std::string& data);
private:
	void inflate(const std::string& data);
	std::vector<std::string> titles_;
};

#endif /* LISTRESPONSE_H_ */
