/*

    .-..-..-----. .--. .-..-. .--.   ,-.
    : :; :`-. .-': .--': :: :: .--'.'  :
    :    :  : :  `. `. : :: :: `;   `: :
    : :: :  : :   _`, :: :; :: :__   : :
    :_;:_;  :_;  `.__.'`.__.'`.__.'  :_;


    By Benjamin Steindl & Patrick Schwab

    File: WelcomeServer.cpp

*/

#include "WelcomeServer.h"

WelcomeServer::WelcomeServer(int port) :
	welcome_socket_(-1),
	buffer_size_(-1),
	queue_size_(-1),
	port_(port)
{
}

WelcomeServer::~WelcomeServer()
{
	shutdown();
}

void WelcomeServer::run()
{
	DEBUG("running server");

	welcome_socket_ = socket(AF_INET, SOCK_STREAM, 0);

	if( welcome_socket_ == -1 )
	{

		throw NetworkException(errno);
		errno = 0;
	}

	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));

	address.sin_family      = AF_INET;
	address.sin_port        = htons(port_);
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	if( bind(welcome_socket_, (struct sockaddr *) &address, sizeof(address)) == -1)
	{
		DEBUG(strerror(errno) << ":: port: " << port_);
		throw NetworkException(errno);
		errno = 0;
	}

	mainLoop();
}

void WelcomeServer::setBufferSize(int bufferSize)
{
	buffer_size_ = bufferSize;
}

void WelcomeServer::setQueueSize(int queueSize)
{
	queue_size_ = queueSize;
}

void WelcomeServer::mainLoop()
{
	if( listen(welcome_socket_, queue_size_ == -1 ? DEFAULT_QUEUE_SIZE : queue_size_) == -1 )
	{
		DEBUG(strerror(errno));
		throw NetworkException(errno);
		errno = 0;
	}

	socklen_t address_length = sizeof(struct sockaddr_in);
	struct sockaddr_in client_address_;

	DEBUG("Listening on port: " << port_);

	while(true)
	{
		int client_socket = accept( welcome_socket_, (struct sockaddr*)&client_address_, &address_length);

		if( client_socket == -1 )
		{
			DEBUG(strerror(errno));
			throw NetworkException(errno);
			errno = 0;
		}
		else
		{
			clients_.push_back(client_socket);

			try
			{
				// create thread here
				handleClient(client_socket);
			}
			catch(NetworkException& e)
			{
				DEBUG(e.what());
				closeSocket(client_socket);
			}
		}
	}
}

void WelcomeServer::handleClient(int sd)
{
	// instantiate the client-handling mailServer
	MailServer ms(sd);

	DEBUG("user connected with id: " << sd);

	int buffer_len = buffer_size_ == -1 ? DEFAULT_BUFFER_SIZE : buffer_size_;
	std::vector<char> buffer(buffer_len);

	ms.clientConnected();

	while(true)
	{
		int size = readline(sd, &buffer[0], buffer_len);

		if( size > 0 )        // properly received message
		{
			DEBUG("Client[" << sd << "] received " << size << " bytes of data.");

			int chop_size = 1; // strip newlines

			if(buffer[size - 2] == '\r')
			{
				chop_size = 2; // strip carriage returns aswell
			}

			std::string* tmp = new std::string(buffer.begin(), buffer.begin() + size - chop_size);
			boost::shared_ptr<std::string> buffer_string(tmp);

			ms.clientReceivedData(buffer_string);
		}
		else if( size == 0 )  // orderly shutdown
		{
			DEBUG("Client[" << sd << "] closed connection.");
			ms.clientDisconnected();
			break;
		}
		else if( size == -1 ) // error
		{
			if(errno != EBADF)
			{
				DEBUG("Client[" << sd << "] terminated with message: "<< strerror(errno));
				ms.clientDisconnected();
			}

			errno = 0;
			break;
		}
	}

	auto it = clients_.begin();
	for(; it != clients_.end(); ++it)
	{
		if(*it == sd)
		{
			break;
		}
	}

	if(it != clients_.end())
	{
		clients_.erase(it);
		closeSocket(sd);
	}
}

void WelcomeServer::shutdown()
{
	if(close(welcome_socket_) == -1)
	{
		DEBUG("Server failed to close the initial connection listener socket, because: "
				<< strerror(errno));
	}

	welcome_socket_ = -1;

	foreach(int client, clients_)
	{
		closeSocket(client);
	}

	clients_.clear();
}
