/*
 * ResponseParser.h
 *
 *  Created on: Oct 10, 2011
 *      Author: patrick
 */

#ifndef RESPONSEPARSER_H_
#define RESPONSEPARSER_H_

#include <string>
#include <vector>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include "../messages/Messages.h"
#include "responses/Responses.h"

class ResponseParser
{
public:
	template <typename T>
	ResponseParser(int socket_id, T* delegate, void (T::*delegate_method)(int, boost::shared_ptr<Response>)) :
		socket_id_(socket_id),
		parser_state_(MessageParserStateNewRequest),
		total_lines_(0)
	{
		this->setDelegate(delegate, delegate_method);
	}

	void digest(boost::shared_ptr<std::string> data, Message::MessageType request_type);
private:
	ResponseParser(const ResponseParser& rp)
	{
		;
	}

	enum MessageParserState
	{
		MessageParserStateNewRequest,
		MessageParserStateParsing
	};

	int socket_id_;
	MessageParserState parser_state_;
	int total_lines_;
	boost::function<void(int, boost::shared_ptr<Response>)>  delegate_method_;
	std::vector< boost::shared_ptr<std::string> > current_response_data_;

	template<typename T, typename R>
	void setDelegate(T* delegate, void (T::*delegate_method)(int, boost::shared_ptr<R>))
	{
		delegate_method_ = boost::bind(delegate_method, delegate, _1, _2);
	}
};

#endif /* RESPONSEPARSER_H_ */
