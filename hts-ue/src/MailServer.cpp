/*

    .-..-..-----. .--. .-..-. .--.   ,-.
    : :; :`-. .-': .--': :: :: .--'.'  :
    :    :  : :  `. `. : :: :: `;   `: :
    : :: :  : :   _`, :: :; :: :__   : :
    :_;:_;  :_;  `.__.'`.__.'`.__.'  :_;


    By Benjamin Steindl & Patrick Schwab

    File: MailServer.cpp

*/

#include "MailServer.h"
#include "Logging.h"

MailServer::MailServer(int sd) :
	fileManager_(0),
	parser_(sd, this, &MailServer::messageReceived),
	socket_id_(sd),
	loginManager_()
{
}

void MailServer::setAbstractFileManager(AbstractFileManager* instance)
{
	fileManager_ = instance;
}

bool MailServer::didFinishMessage()
{
	return parser_.getParserState() == MessageParser::MessageParserStateNewRequest ? true : false;
}

void MailServer::clientConnected()
{
	ClientInfo* current_client = ClientRestrictionManager::getInstance()->getCurrentClient();

	if( current_client->isLocked() )
	{
		closeSocket( current_client->getSocketDescriptor() );
	}
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
		case Message::MessageTypeLogin:
			handleLogin(static_cast<LoginMessage&>(*msg));
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
			fileManager_->persistSendMessage(msg);
			sendOk(socket_id_);
		}
		catch(const FileManagerException& e)
		{
			sendErr(socket_id_);
		}
	}
	catch(const NetworkException& e)
	{
		DEBUG("Failed to send reply for sendMessage request, because " << e.what());
	}
}

void MailServer::handleList(const ListMessage& msg)
{
	std::vector<std::string> list;

	try
	{
		list = fileManager_->getMessageList(msg);
	}
	catch(const FileManagerException& e)
	{
		DEBUG("Failed to obtain message list, because " << e.what());
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
		auto pMsg = fileManager_->getMessageForRead(msg);

		sendOk(socket_id_);

		auto str = pMsg->deflate();

		sendMsg(socket_id_, *str);
	}
	catch(const FileManagerException& e)
	{
		DEBUG("Failed to get message for username "
				<< msg.username_ << " and index "
				<< msg.message_number_ << ", because "
				<< e.what());

		sendErr(socket_id_);
	}
}

void MailServer::handleDel(const DelMessage& msg)
{
	try
	{
		fileManager_->removeFile(msg);

		sendOk(socket_id_);
	}
	catch(const FileManagerException& e)
	{
		DEBUG("Failed to delete message for username "
				<< msg.username_ << " and index "
				<< msg.message_number_ << ", because "
				<< e.what());

		sendErr(socket_id_);
	}
}

void MailServer::handleQuit(const QuitMessage& msg)
{
	closeSocket(socket_id_);
}

void MailServer::handleLogin(const LoginMessage& msg)
{
	bool retVal = loginManager_.sendLDAPRequest(msg.username_, msg.password_);

	if(retVal)
	{
		sendOk(socket_id_);
	}
	else
	{
		ClientInfo* ci = ClientRestrictionManager::getInstance()->getCurrentClient();

		if( ci->isLocked() )
		{
			closeSocket(socket_id_);
		}
		else
		{
			sendErr(socket_id_);
		}
	}
}

