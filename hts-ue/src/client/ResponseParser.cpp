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
			current_response_data_.clear();

			switch(request_type)
			{
				case Message::MessageTypeSend:
				case Message::MessageTypeDel:
				case Message::MessageTypeQuit:
				case Message::MessageTypeLogin:
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
					else
					{
						boost::shared_ptr<Response> pResp(new ErrResponse());
						delegate_method_(socket_id_, pResp);
					}
					break;
				case Message::MessageTypeList:
					DEBUG("LIST");
					try
					{
						current_response_data_.push_back(data);
						total_lines_ = boost::lexical_cast<int>(str);

						if(total_lines_ > 0)
						{
							parser_state_ = ResponseParser::MessageParserStateParsing;
						}
						else
						{
							boost::shared_ptr<Response> pResp(new ListResponse(str));

							delegate_method_(socket_id_, pResp);
						}
					}
					catch(const boost::bad_lexical_cast& e)
					{
						boost::shared_ptr<Response> pResp(new ErrResponse());
						delegate_method_(socket_id_, pResp);
					}
					break;
				case Message::MessageTypeRead:
					if(str == "OK")
					{
						parser_state_ = ResponseParser::MessageParserStateParsing;
					}
					else if(str == "ERR")
					{
						boost::shared_ptr<Response> pResp(new ErrResponse());
						delegate_method_(socket_id_, pResp);
					}
					else
					{
						boost::shared_ptr<Response> pResp(new ErrResponse());
						delegate_method_(socket_id_, pResp);
					}
					break;
				case Message::MessageTypeInvalid:
				default:
					DEBUG("Impossible client state. MsgType is " << request_type);
					break;
			}
			break;
		case ResponseParser::MessageParserStateParsing:
			if(request_type == Message::MessageTypeList)
			{
				current_response_data_.push_back(data);
				total_lines_--;

				if(total_lines_ == 0)
				{
					std::string total_data;
					foreach(auto tmp, current_response_data_)
					{
						total_data += *tmp + "\n";
					}

					boost::shared_ptr<Response> pResp;

					try
					{
						pResp.reset(new ListResponse(total_data));
					}
					catch(const ConversionException& e)
					{
						pResp.reset(new ErrResponse());
					}

					delegate_method_(socket_id_, pResp);
					parser_state_ = ResponseParser::MessageParserStateNewRequest;
				}
			}
			else if(request_type == Message::MessageTypeRead)
			{
				message_parser_.digest(data);

				if(message_parser_.getParserState() == MessageParser::MessageParserStateNewRequest &&
				   didReceiveMsg_ == false)
				{
					boost::shared_ptr<Response> pResp(new ErrResponse());

					delegate_method_(socket_id_, pResp);
					parser_state_ = ResponseParser::MessageParserStateNewRequest;
				}

				if(didReceiveMsg_ == true)
				{
					didReceiveMsg_ = false;
					boost::shared_ptr<Response> pResp(new ReadResponse(tmp_msg_));

					delegate_method_(socket_id_, pResp);
					parser_state_ = ResponseParser::MessageParserStateNewRequest;
				}
			}
			break;
		default:
			DEBUG("Impossible parser state.");
			break;
	}
}

void ResponseParser::finishedSendMessage(int sd, boost::shared_ptr<Message> msg)
{
	if(msg->getType() == Message::MessageTypeSend)
	{
		tmp_msg_ = static_cast<SendMessage&>(*msg);

		didReceiveMsg_ = true;
	}
	else
	{
		didReceiveMsg_ = false;
	}
}

int ResponseParser::getAwaitingSize()
{
	return message_parser_.getAwaitingSize();
}
