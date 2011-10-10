/*
 * Client.cpp
 *
 *  Created on: Oct 10, 2011
 *      Author: patrick
 */

#include "Client.h"

Client::Client(int port, std::string ip_address) :
	ip_address_(ip_address == "localhost" ? "127.0.0.1" : ip_address),
	port_(port),
	socket_(-1),
	responseParser_(socket_, this, &Client::didReceiveResponse)
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
	try
	{
		auto pstr = msg.deflate();

		int toSend = pstr->size();

		while(toSend != 0)
		{
			int amount = send(socket_, &(pstr->c_str())[pstr->size() - toSend], toSend, 0);

			DEBUG("sending.. " << amount);

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
		int size = readline(socket_, &buffer[0], buffer_len);

		if( size > 0 )        // properly received data
		{
			DEBUG("Client[" << socket_ << "] received " << size << " bytes of data.");

			int chop_size = 1; // strip newlines

			if(buffer[size - 2] == '\r')
			{
				chop_size = 2; // strip carriage returns aswell
			}

			std::string* tmp = new std::string(buffer.begin(), buffer.begin() + size - chop_size);

			boost::shared_ptr<std::string> buffer_string(tmp);
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

	char ctrl;
	while(ctrl != '5')
	{
		switch(ctrl = printMenu())
		{
			case '1': // SEND
			{
				SendMessage sm;

				readParam("Please enter the receiver's name.", sm.receiver_);

				readParam("Please enter your name.", sm.sender_);

				readParam("Please enter the message's title.", sm.title_);

				readStringUntil("Please enter the message you want to transmit. Terminate with $.",
								sm.body_,
								'$');

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
				QuitMessage qm;

				std::cout << std::endl << "  " << "You requested the server to close the connection." << std::endl;

				transmitMessage(qm);
				break;
			}
			default:
				std::cout << std::endl << "  " << "Please choose a valid menu entry." << std::endl;
				break;
		}
	}
}

char Client::printMenu()
{
	std::vector<std::string> collection;

	collection.push_back("Send Message");
	collection.push_back("List Messages");
	collection.push_back("Read Message");
	collection.push_back("Delete Message");
	collection.push_back("Quit Message");

	std::cout << std::endl << "   === MENU ===" << std::endl << std::endl;

	for(unsigned int i = 0; i < collection.size(); ++i)
	{
		std::cout << "     [" << i + 1 << "] ... " << collection[i] << std::endl;
	}

	std::cout << std::endl;

	char choice;
	std::cin >> choice;

	return choice;
}
