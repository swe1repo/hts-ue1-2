/*
 * MessageParser.cpp
 *
 *  Created on: Sep 30, 2011
 *      Author: patrick
 */

#include "MessageParser.h"

void MessageParser::digest(boost::shared_ptr<std::string> data)
{
	switch(parser_state_)
	{
		case MessageParser::MessageParserStateNewRequest:
			message_type_ = message_factory_.getMessageType(*data);

			if(message_type_ != Message::MessageTypeInvalid)
			{
				if(message_type_ != Message::MessageTypeQuit)
				{
					parser_state_ = MessageParser::MessageParserStateParsing;
				}

				current_message_data_.push_back(data);
			}

			switch(message_type_)
			{
				case Message::MessageTypeSend:
					total_lines_ = 3;
					break;
				case Message::MessageTypeList:
					total_lines_ = 1;
					break;
				case Message::MessageTypeRead:
				case Message::MessageTypeDel:
					total_lines_ = 2;
					break;
				case Message::MessageTypeQuit:
					{
						// Quit message doesn't have any data content
						Message* finished_message = message_factory_.newInstance(current_message_data_);

						current_message_data_.clear();

						delegate_method_(1, boost::shared_ptr<Message>(finished_message));
						return;
					}
				case Message::MessageTypeInvalid:
				default:
					DEBUG("Received a message of unknown type: " << *data);
					break;
			}
			break;
		case MessageParser::MessageParserStateParsing:
			current_message_data_.push_back(data);
			total_lines_--;

			if(message_type_ == Message::MessageTypeSend)
			{
				if(total_lines_ < 0 && data->compare(".") == 0)
				{
					// send message ends here
					parser_state_ = MessageParser::MessageParserStateNewRequest;
				}
			}
			else
			{
				if(total_lines_ == 0)
				{
					// message ends here
					parser_state_ = MessageParser::MessageParserStateNewRequest;
				}
			}

			if(parser_state_ == MessageParser::MessageParserStateNewRequest)
			{
				// dispatch a finished message to the delegate
				Message* finished_message = message_factory_.newInstance(current_message_data_);

				// clear message data, get ready for a new run
				current_message_data_.clear();

				delegate_method_(1, boost::shared_ptr<Message>(finished_message));
			}

			break;
		default:
			DEBUG("Parser is in impossible parser state.");
			break;
	}
}
