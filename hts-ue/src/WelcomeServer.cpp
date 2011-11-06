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
	shut_down();

	delete ClientRestrictionManager::getInstance();
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

	// allow socket re-use
	int flag = 0;
	setsockopt(welcome_socket_, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

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

	// initialize timer
	time_t last_update = time(0);

	while(true)
	{
		int client_socket = accept( welcome_socket_, (struct sockaddr*)&client_address_, &address_length);

		// create new clientInfo instance - is going to be passed to thread specific pointer
		// thus it is automatically deleted on thread-exit.
		ClientInfo* ci = new ClientInfo(client_socket, client_address_);

		// update timer
		ClientRestrictionManager::getInstance()->update( time(0) - last_update );
		last_update = time(0);

		if( client_socket == -1 )
		{
			DEBUG(strerror(errno));
			throw NetworkException(errno);
			errno = 0;
		}
		else
		{
			try
			{
				client_sockets_.push_back(client_socket);
				clients_.create_thread(boost::bind(&WelcomeServer::handleClient,
												   this,
												   ci));
			}
			catch(NetworkException& e)
			{
				DEBUG(e.what());
				closeSocket(client_socket);
			}
		}
	}
}

void WelcomeServer::handleClient(ClientInfo* client_info)
{
	int sd = client_info->getSocketDescriptor();

	// set this threads handled client
	ClientRestrictionManager::getInstance()->setCurrentClient(client_info);

	// instantiate the client-handling mailServer
	MailServer ms( sd );

	ms.setAbstractFileManager(ThreadedFileManager::getInstance());

	DEBUG("user connected with id: " << client_info->getSocketDescriptor());

	int buffer_len = buffer_size_ == -1 ? DEFAULT_BUFFER_SIZE : buffer_size_;
	std::vector<char> buffer(buffer_len);

	ms.clientConnected();

	while(true)
	{
		int size;

		if( (size = ms.getAwaitingSize()) != 0)
		{
			buffer.reserve(size);

			for(int i = 0; i < size; i++)
			{
				my_read(sd, &buffer[i]);
			}
		}
		else
		{
			size = readline(sd, &buffer[0], buffer_len);
		}

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

		try
		{
			// the server may be interrupted only at this point
			boost::this_thread::interruption_point();
		}
		catch(const boost::thread_interrupted& e)
		{
			// allow the thread to finish the message it is currently working on -
			// so that no half-sent messages may be lost in the destruction process.
			if(ms.didFinishMessage())
			{
				break;
			}
			else
			{
				continue;
			}
		}
	}

	// execution finished, for whatever reason -> close the clients socket
	closeSocket( client_info->getSocketDescriptor() );
}

void WelcomeServer::shut_down()
{
	// shut down the welcome socket
	closeSocket(welcome_socket_);

	welcome_socket_ = -1;

	// end all running threads/clients here
	clients_.interrupt_all();

	foreach(int client, client_sockets_)
	{
		if(shutdown(client, SHUT_RDWR) == -1)
		{
			if(errno == EBADF)
			{
				DEBUG("Client[" << client << "] was already closed.");
			}
		}
	}

	clients_.join_all();
}
