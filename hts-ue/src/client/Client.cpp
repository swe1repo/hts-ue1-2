/*
 * Client.cpp
 *
 *  Created on: Oct 10, 2011
 *      Author: patrick
 */

#include "Client.h"

namespace fs = boost::filesystem;

Client::Client(int port, std::string ip_address) :
	ip_address_(ip_address == "localhost" ? "127.0.0.1" : ip_address),
	port_(port),
	socket_(-1),
	response_parser_(socket_, this, &Client::didReceiveResponse),
	last_type_(Message::MessageTypeInvalid),
	end_receiving_(false),
	userManager_()
{
}

Client::~Client()
{
	if(close(socket_) == -1)
	{
		DEBUG("Failed to close socket [" << socket_ << "], because: " << strerror(errno));
	}
}

void Client::transmitMessage(const Message& msg)
{
	last_type_ = msg.getType();

	try
	{
		auto pstr = msg.deflate();

		int toSend = pstr->size();

		while(toSend != 0)
		{
			int amount = send(socket_, &(pstr->c_str())[pstr->size() - toSend], toSend, 0);

			if(amount == -1)
			{
				if(errno == EINTR)
					continue;

				throw NetworkException(errno);
			}
			else
			{
				toSend -= amount;
			}
		}

	}
	catch(const ConversionException& e)
	{
		throw NetworkException();
	}
}

void Client::getResponse()
{
	const int buffer_len = DEFAULT_BUFFER_SIZE;
	std::vector<char> buffer(buffer_len);

	while(true)
	{
		int size;
		if( (size = response_parser_.getAwaitingSize()) != 0 )
		{
			buffer.reserve(size);

			for(int i = 0; i < size; i++)
			{
				my_read(socket_, &buffer[i]);
			}
		}
		else
		{
			size = readline(socket_, &buffer[0], buffer_len);
		}

		if( size > 0 )        // properly received data
		{
			int chop_size = 1; // strip newlines

			if(buffer[size - 2] == '\r')
			{
				chop_size = 2; // strip carriage returns aswell
			}

			std::string* tmp = new std::string(buffer.begin(), buffer.begin() + size - chop_size);

			boost::shared_ptr<std::string> buffer_string(tmp);

			response_parser_.digest(buffer_string, last_type_);

			if(end_receiving_ == true)
			{
				end_receiving_ = false;
				break;
			}
		}
		else if( size == 0 )  // orderly shutdown
		{
			DEBUG("Server[" << socket_ << "] closed connection.");
			throw NetworkException();
			break;
		}
		else if( size == -1 ) // error
		{
			if(errno != EBADF)
			{
				DEBUG("Server[" << socket_ << "] terminated with message: "<< strerror(errno));
				throw NetworkException(errno);
			}

			errno = 0;
			break;
		}
	}
}

void Client::didReceiveResponse(int socket, boost::shared_ptr<Response> response)
{
	end_receiving_ = true;

	switch(response->getType())
	{
		case Response::ResponseTypeOk:
			if(userManager_.getUsername().empty() == false &&
			   userManager_.isLoggedIn() == false)
			{
				userManager_.loginSucceeded();
			}

			std::cout << "  " << "Received Response: OK" << std::endl << std::endl;
			break;
		case Response::ResponseTypeErr:
			if(userManager_.getUsername().empty() == false &&
			   userManager_.isLoggedIn() == false)
			{
				userManager_.loginFailed();
			}

			std::cout << "  " << "Received Response: ERR" << std::endl << std::endl;
			break;
		case Response::ResponseTypeList:
		{
			std::cout << "  " << "Received Response: LIST with "
					  << static_cast<ListResponse&>(*response).titles_.size()
					  << " entries." << std::endl << std::endl;

			auto vec = static_cast<ListResponse&>(*response).titles_;
			for(unsigned int i = 0; i < vec.size(); ++i)
			{
				std::cout << "  " << "[ " << i << " ] .. Entry: " << vec[i] << std::endl;
			}
			break;
		}
		case Response::ResponseTypeRead:
		{
			std::cout << "  " << "Received Response: READ" << std::endl << std::endl;

			SendMessage sm = static_cast<ReadResponse&>(*response).getMessage();

			std::cout << " - - - - - - - - - - - - - - - - - - - - - - - " << std::endl;
			std::cout << "  " << "-- Sender: "   << sm.sender_   << std::endl;
			std::cout << "  " << "-- Receivers: " << std::endl;

			for(unsigned int i = 0; i < sm.receivers_.size(); ++i)
			{
				std::cout << "   [" << i << "] -> " << sm.receivers_[i] << std::endl;
			}

			std::cout << "  " << "-- Title: "    << sm.title_    << std::endl;
			std::cout << " - - - - - - - - - - - - - - - - - - - - - - - " << std::endl;
			std::cout << "  " << "-- Body: " << sm.body_ << std::endl;
			std::cout << " - - - - - - - - - - - - - - - - - - - - - - - " << std::endl;
			break;
		}
		default:
			DEBUG("Unknown response received. Discarding.");
			break;
	}
}

bool Client::connectToServer()
{
	struct sockaddr_in address;

	if ((socket_ = socket (AF_INET, SOCK_STREAM, 0)) == -1)
	{
		DEBUG("Failed to open socket, because: " << strerror(errno));
		return false;
	}

	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(port_);
	inet_aton(ip_address_.c_str(), &address.sin_addr);

	if( connect( socket_, (struct sockaddr *)&address, sizeof (address)) == 0)
	{
		std::cout << std::endl << "  " << "Connection with server at "
				  << inet_ntoa (address.sin_addr)
				  << ":" << port_ << " established" << std::endl;
	}
	else
	{
		DEBUG("Connect error - no server available, because " << strerror(errno));
	 	return false;
	}

	return true;
}

void Client::run()
{
	if(!connectToServer())
	{
		throw NetworkException(errno);
	}

	bool doQuit = false;

	while(!doQuit)
	{
		try
		{
			if(userManager_.isLoggedIn())
			{
				doQuit = presentMainMenu();
			}
			else
			{
				doQuit = presentLoginMenu();
			}
		}
		catch(const InvalidEntryException& e)
		{
			DEBUG(e.what());
			continue;
		}

		// receive the servers response -- blocking
		getResponse();
	}
}

bool Client::presentMainMenu()
{
	char ctrl;
	bool doQuit = false;

	switch(ctrl = printMenu())
	{
		case '1': // SEND
		{
			SendMessage sm;

			do
			{
				std::string tmp;
				readParam("Please enter the receivers' names.", tmp);
				sm.receivers_.push_back(tmp);
			}
			while( readYesNoQuestion("Do you want to enter another receiver? (y/n)") == true );

			readParam("Please enter your name.", sm.sender_);

			readParam("Please enter the message's title.", sm.title_);

			readStringUntil("Please enter the message you want to transmit. Terminate with a dot '.' .",
							sm.body_);

			if( readYesNoQuestion("Do you want to add an attachment? (y/n)") == true )
			{
				do
				{
					std::string file_path;
					readParam("Enter the path to the file you wish to attach.", file_path);

					try
					{
						sm.attachments_.push_back( readAttachment(file_path) );
					}
					catch(const ConversionException& e)
					{
						std::cout << std::endl << "  " << "Failed to attach the file, because " <<
								  e.what() << std::endl;
					}
				}
				while( readYesNoQuestion("Do you want to add another attachment? (y/n)") == true );
			}

			transmitMessage(sm);
			break;
		}
		case '2': // LIST
		{
			ListMessage lm;

			readParam("Please enter the username for whom you want to request a message list.", lm.username_);

			transmitMessage(lm);
			break;
		}
		case '3': // READ
		{
			ReadMessage rm;

			readParam("Please enter the receiver's username.", rm.username_);

			readParam("Please enter the number of the message to display. [0 based index]", rm.message_number_);

			transmitMessage(rm);
			break;
		}
		case '4': // DEL
		{
			DelMessage dm;

			readParam("Please enter the receiver's username.", dm.username_);

			readParam("Please enter the number of the message to delete. [0 based index]", dm.message_number_);

			transmitMessage(dm);
			break;
		}
		case '5': // QUIT
		{
			createAndSendQuitMessage();

			doQuit = true;
			break;
		}
		default:
			std::string error_text("Please choose a valid menu entry.");
			std::cout << std::endl << "  " << error_text << std::endl;
			throw InvalidEntryException(error_text);
			break;
	}

	return doQuit;
}

bool Client::presentLoginMenu()
{
	char ctrl;
	bool doQuit = false;

	switch(ctrl = printLoginMenu())
	{
		case '1':
		{
			LoginMessage lm;

			readParam("Please enter your username.", lm.username_);

			readParamHidden("Please enter your password.", lm.password_);

			transmitMessage(lm);

			userManager_.startLogin(lm.username_);

			break;
		}
		case '2':
			createAndSendQuitMessage();
			doQuit = true;
			break;
		default:
			std::string error_text("Please choose a valid menu entry.");
			std::cout << std::endl << "  " << error_text << std::endl;
			throw InvalidEntryException(error_text);
			break;
	}

	return doQuit;
}

char Client::printLoginMenu()
{
	std::vector<std::string> collection;

	collection.push_back("Login Message");
	collection.push_back("Quit Message");

	return createMenuWithCollection(collection);
}

char Client::printMenu()
{
	std::vector<std::string> collection;

	collection.push_back("Send Message");
	collection.push_back("List Messages");
	collection.push_back("Read Message");
	collection.push_back("Delete Message");
	collection.push_back("Quit Message");

	return createMenuWithCollection(collection);
}

char Client::createMenuWithCollection(const std::vector<std::string>& collection)
{
	std::cout << std::endl << "   === MENU ===" << std::endl << std::endl;

	for(unsigned int i = 0; i < collection.size(); ++i)
	{
		std::cout << "     [" << i + 1 << "] ... " << collection[i] << std::endl;
	}

	std::cout << std::endl;

	char choice;

	readParam("Please choose your option.", choice);

	return choice;
}

void Client::createAndSendQuitMessage()
{
	QuitMessage qm;

	std::cout << std::endl << "  " << "You requested the server to close the connection." << std::endl;

	transmitMessage(qm);
}

Attachment Client::readAttachment(std::string file_path)
{
	Attachment retVal;

	fs::path path( file_path );

	int size = fs::file_size( path );

	if(size == 0)
	{
		throw ConversionException("File at " + file_path + " doesn't exist or can't be read.");
	}

	retVal.data_.reserve(size);

	fs::ifstream ifs;

	ifs.open( path );

	if(ifs.fail())
	{
		throw ConversionException("Failed to open file [ " + path.string() + " ]");
	}

	ifs.read(&retVal.data_[0], size);

	if(ifs.fail())
	{
		throw ConversionException("Failed to read file content. [ " + path.string() + " ]");
	}

	retVal.data_.assign(retVal.data_.begin(), retVal.data_.begin() + size);

	ifs.close();

	return retVal;
}
