/*
 * MailServer.cpp
 *
 *  Created on: Sep 27, 2011
 *      Author: patrick
 */

#include "MailServer.h"
#include "Logging.h"

MailServer::MailServer(int sd) :
	parser_(sd, this, &MailServer::messageReceived),
	socket_id_(sd)
{
}

MailServer::~MailServer()
{
}

void MailServer::clientConnected()
{
}

void MailServer::clientReceivedData(boost::shared_ptr<std::string> data)
{
	DEBUG("Digesting received data: " << *data);

	try
	{
		// the parser will notify the server of completed messages through the delegate method
		parser_.digest(data);
	}
	catch(ConversionException& e)
	{
		DEBUG("Caught conversionException.");
		sendErr(socket_id_);
	}
}

void MailServer::messageReceived(int sd, boost::shared_ptr<Message> msg)
{
	DEBUG("A message has been dispatched!");
	switch((*msg).getType())
	{
		case Message::MessageTypeSend:
			handleSend(static_cast<SendMessage&>(*msg));
			break;
		case Message::MessageTypeList:
			handleList(static_cast<ListMessage&>(*msg));
			break;
		case Message::MessageTypeRead:
			handleRead(static_cast<ReadMessage&>(*msg));
			break;
		case Message::MessageTypeDel:
			handleDel(static_cast<DelMessage&>(*msg));
			break;
		case Message::MessageTypeQuit:
			handleQuit(static_cast<QuitMessage&>(*msg));
			break;
		case Message::MessageTypeInvalid:
		default:
			DEBUG("Ignoring: An invalid/unknown message type was received. Code: [" << (*msg).getType() << "].");
			break;
	}
}

void MailServer::clientDisconnected()
{
}

void MailServer::clientConnectionTerminated()
{
}

void MailServer::handleSend(const SendMessage& msg)
{
	try
	{
		try
		{
			FileManager::getInstance()->persistSendMessage(msg);
			sendOk(socket_id_);
		}
		catch(const FileManagerException& e)
		{
			sendErr(socket_id_);
		}
	}
	catch(const NetworkException& e)
	{
		DEBUG("Failed to send reply for sendMessage request.");
	}
}

void MailServer::handleList(const ListMessage& msg)
{
	std::vector<std::string> list;

	try
	{
		list = FileManager::getInstance()->getMessageList(msg);
	}
	catch(const FileManagerException& e)
	{
		DEBUG("Failed to obtain message list.");
	}

	try
	{
		sendMsg(socket_id_, boost::lexical_cast<std::string>(list.size()) + "\n");
	}
	catch(const boost::bad_lexical_cast& e)
	{
		DEBUG("Failed to convert to string value.");
	}

	foreach(std::string str, list)
	{
		sendMsg(socket_id_, str + "\n");
	}
}

void MailServer::handleRead(const ReadMessage& msg)
{
	try
	{
		auto pMsg = FileManager::getInstance()->getMessageForRead(msg);

		sendOk(socket_id_);

		auto str = pMsg->deflate();

		sendMsg(socket_id_, *str);
	}
	catch(const FileManagerException& e)
	{
		DEBUG("Failed to get message for username "
				<< msg.username_ << " and index "
				<< msg.message_number_ << ".");

		sendErr(socket_id_);
	}
}

void MailServer::handleDel(const DelMessage& msg)
{
	try
	{
		FileManager::getInstance()->removeFile(msg);

		sendOk(socket_id_);
	}
	catch(const FileManagerException& e)
	{
		DEBUG("Failed to delete message for username "
				<< msg.username_ << " and index "
				<< msg.message_number_ << ".");

		sendErr(socket_id_);
	}
}

void MailServer::handleQuit(const QuitMessage& msg)
{
	closeSocket(socket_id_);
}

