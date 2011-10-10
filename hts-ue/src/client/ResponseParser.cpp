/*
 * ResponseParser.cpp
 *
 *  Created on: Oct 10, 2011
 *      Author: patrick
 */

#include "ResponseParser.h"

void ResponseParser::digest(boost::shared_ptr<std::string> data, Message::MessageType request_type)
{
	auto str = *data;

	switch(parser_state_)
	{
	case ResponseParser::MessageParserStateNewRequest:
		switch(request_type)
		{
		case Message::MessageTypeSend:
		case Message::MessageTypeDel:
		case Message::MessageTypeQuit:
			if(str == "OK")
			{
				boost::shared_ptr<Response> pResp(new OkResponse());
				delegate_method_(socket_id_, pResp);
			}
			else if(str == "ERR")
			{
				boost::shared_ptr<Response> pResp(new ErrResponse());
				delegate_method_(socket_id_, pResp);
			}
			break;
		case Message::MessageTypeList:
			try
			{
				total_lines_ = boost::lexical_cast<int>(str);
				parser_state_ = ResponseParser::MessageParserStateParsing;
			}
			catch(const boost::bad_lexical_cast& e)
			{
				DEBUG("Bad cast.");
			}
			break;
		case Message::MessageTypeRead:

		case Message::MessageTypeInvalid:
		default:
			DEBUG("Impossible client state.");
			break;
		}
	case ResponseParser::MessageParserStateParsing:
		if(request_type == Message::MessageTypeList)
		{

		}
		else if(request_type == Message::MessageTypeRead)
		{

		}
		break;
	default:
		DEBUG("Impossible parser state.");
		break;
	}
}

