/*
 * SendMessage.cpp
 *
 *  Created on: Sep 29, 2011
 *      Author: patrick
 */

#include "SendMessage.h"

SendMessage::SendMessage()
{
}

SendMessage::SendMessage(const std::string& data)
{
	inflate(data);
}

void SendMessage::inflate(const std::string& data)
{
	std::stringstream ss(data);
	std::string line;

	std::getline(ss, line, '\n');

	if(line.compare("SEND") != 0)
	{
		std::string error_text = "Inflation failed, because the supplied message data is not of the proper type.";

		DEBUG(error_text);
		throw ConversionException(error_text);
	}

	std::getline(ss, line, '\n');
	sender_ = line;

	char eot[] = EOT_STRING;

	while(true)
	{
		std::getline(ss, line, '\n');

		if(line.compare(&eot[0]) == 0)
		{
			// receiver list ended
			break;
		}
		else
		{
			// add to receivers
			receivers_.push_back(line);
		}
	}

	std::getline(ss, line, '\n');
	title_ = line;

	while(true)
	{
		std::getline(ss, line, '\n');

		if(line.compare(".") == 0)
		{
			// message body ended
			break;
		}
		else
		{
			// re-append stripped newlines
			body_.append(line + "\n");
		}
	}

	std::getline(ss, line, '\n');

	int num_attachments;
	try
	{
		attachments_.reserve( num_attachments = boost::lexical_cast<int>(line) );
		DEBUG(line << " is conv to " << num_attachments);
	}
	catch(const boost::bad_lexical_cast& e)
	{
		std::string error_text = "Inflation failed, because the supplied message data is not of the proper type.";
		DEBUG(error_text);

		throw ConversionException(error_text);
	}

	for(int i = 0; i < num_attachments; i++)
	{
		Attachment tmp;

		std::getline(ss, line, '\n');

		int file_size;
		try
		{
			tmp.data_.reserve( file_size = boost::lexical_cast<int>(line) );
			DEBUG(line << " is conv to " << file_size);
		}
		catch(const boost::bad_lexical_cast& e)
		{
			std::string error_text = "Inflation failed, because the supplied message data is not of the proper type.";
			DEBUG(error_text);

			throw ConversionException(error_text);
		}


		ss.read( &tmp.data_[0], file_size );

		if(ss.fail())
		{
			throw ConversionException("Serialized Message did not contain as much data as it should.");
		}

		DEBUG("DATA:: " << &tmp.data_[0]);

		tmp.data_.assign(tmp.data_.begin(),
						 tmp.data_.begin() + file_size);

		attachments_.push_back(tmp);
	}

	DEBUG("ATT SIZE: " << attachments_.size() << ", ATT AT 0 SIZE: " << attachments_[0].data_.size());
}

boost::shared_ptr<std::string> SendMessage::deflate() const
{
	boost::shared_ptr<std::string> deflated_string(new std::string);

	*deflated_string += "SEND\n" + sender_ + "\n";

	foreach(std::string receiver, receivers_)
	{
		// don't forget re-appending the newlines
		*deflated_string += receiver + "\n";
	}

	char eot[] = EOT_STRING;

	*deflated_string += std::string(&eot[0]) + "\n"  + title_ + "\n" + body_ + ".\n";

	try
	{
		*deflated_string += boost::lexical_cast<std::string>( attachments_.size() ) + "\n";

		foreach(Attachment attachment, attachments_)
		{
			*deflated_string += boost::lexical_cast<std::string>( attachment.data_.size() ) + "\n";
			*deflated_string += std::string( attachment.data_.begin(), attachment.data_.end() );
		}
	}
	catch(boost::bad_lexical_cast& e)
	{
		std::string error_text = "Deflation failed, because the supplied message data is not of the proper format.";

		DEBUG(error_text);
		throw ConversionException(error_text);
	}

	return deflated_string;
}

SendMessage::MessageType SendMessage::getType() const
{
	return Message::MessageTypeSend;
}
