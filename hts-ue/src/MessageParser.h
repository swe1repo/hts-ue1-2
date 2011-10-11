/*

    .-..-..-----. .--. .-..-. .--.   ,-.
    : :; :`-. .-': .--': :: :: .--'.'  :
    :    :  : :  `. `. : :: :: `;   `: :
    : :: :  : :   _`, :: :; :: :__   : :
    :_;:_;  :_;  `.__.'`.__.'`.__.'  :_;


    By Benjamin Steindl & Patrick Schwab

    File: MessageParser.h

*/

#ifndef MESSAGEPARSER_H_
#define MESSAGEPARSER_H_

#include <string>
#include <vector>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include "messages/Messages.h"
#include "messages/MessageFactory.h"

class MessageParser
{
public:
	enum MessageParserState
	{
		MessageParserStateNewRequest,
		MessageParserStateParsing
	};

	template <typename T>
	MessageParser(int socket_id, T* delegate, void (T::*delegate_method)(int, boost::shared_ptr<Message>)) :
		socket_id_(socket_id),
		parser_state_(MessageParserStateNewRequest),
		total_lines_(0)
	{
		this->setDelegate(delegate, delegate_method);
	}

	void digest(boost::shared_ptr<std::string> data);
	MessageParserState getParserState();
private:
	Message::MessageType getMessageType(std::string line);

	template<typename T, typename R>
	void setDelegate(T* delegate, void (T::*delegate_method)(int, boost::shared_ptr<R>))
	{
		delegate_method_ = boost::bind(delegate_method, delegate, _1, _2);
	}

	// client data
	boost::function<void(int, boost::shared_ptr<Message>)> delegate_method_;
	int socket_id_;

	// parser data
	MessageFactory message_factory_;
	MessageParserState parser_state_;
	Message::MessageType message_type_;
	int total_lines_;
	std::vector< boost::shared_ptr<std::string> > current_message_data_;
};

#endif /* MESSAGEPARSER_H_ */
