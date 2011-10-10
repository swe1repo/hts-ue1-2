/*
 * ListResponse.cpp
 *
 *  Created on: Oct 10, 2011
 *      Author: patrick
 */

#include "ListResponse.h"

ListResponse::ListResponse(const std::string& data) :
	Response(Response::ResponseTypeRead)
{
	inflate(data);
}

void ListResponse::inflate(const std::string& data)
{
	std::stringstream ss(data);
	std::string line;

	int title_count = 0;
	std::getline(ss, line, '\n');
	try
	{
		title_count = boost::lexical_cast<int>(line);
	}
	catch(const boost::bad_lexical_cast& e)
	{
		throw ConversionException("Failed to convert to int.");
	}

	DEBUG("num titles is " << titles_.size());

	while(title_count-- != 0)
	{
		std::getline(ss, line, '\n');
		titles_.push_back(line);
	}
}
