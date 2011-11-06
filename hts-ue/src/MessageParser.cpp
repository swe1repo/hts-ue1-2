/*

    .-..-..-----. .--. .-..-. .--.   ,-.
    : :; :`-. .-': .--': :: :: .--'.'  :
    :    :  : :  `. `. : :: :: `;   `: :
    : :: :  : :   _`, :: :; :: :__   : :
    :_;:_;  :_;  `.__.'`.__.'`.__.'  :_;


    By Benjamin Steindl & Patrick Schwab

    File: MessageParser.cpp

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
				case Message::MessageTypeLogin:
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
					// Message body ends here
					parser_state_ = MessageParser::MessageParserStateAwaitingAttachments;
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
		case MessageParser::MessageParserStateAwaitingAttachments:
			DEBUG("Attachment awaiting. Total lines = " << total_lines_ << ", data: " << *data
					<< ", data size: " << data->size());
			current_message_data_.push_back(data);
			total_lines_--;

			try
			{
				if(total_lines_ < 0)
				{
					total_lines_ = boost::lexical_cast<int>(*data) * 2;
				}
				else if(total_lines_ % 2 == 1)
				{
					awaiting_size_ = boost::lexical_cast<int>(*data);
					DEBUG("Awaiting size: " << awaiting_size_);
				}
				else
				{
					awaiting_size_ = 0;
				}
			}
			catch(const boost::bad_lexical_cast& e)
			{
				DEBUG("Failed to convert input to number. Discarding.");
				parser_state_ = MessageParser::MessageParserStateNewRequest;
			}

			if(total_lines_ == 0)
			{
				// message ends here
				parser_state_ = MessageParser::MessageParserStateNewRequest;
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

MessageParser::MessageParserState MessageParser::getParserState()
{
	return parser_state_;
}

int MessageParser::getAwaitingSize()
{
	return awaiting_size_;
}
