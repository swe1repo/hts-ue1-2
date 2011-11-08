/*

    .-..-..-----. .--. .-..-. .--.   ,-.
    : :; :`-. .-': .--': :: :: .--'.'  :
    :    :  : :  `. `. : :: :: `;   `: :
    : :: :  : :   _`, :: :; :: :__   : :
    :_;:_;  :_;  `.__.'`.__.'`.__.'  :_;


    By Benjamin Steindl & Patrick Schwab

    File: Server.cpp

*/

#include "Server.h"
#include "Logging.h"

Server::Server()
{
}

void Server::closeSocket(int sd)
{
	if(close(sd) == -1)
	{
		if(errno != EBADF)
		{
			DEBUG("Server failed to close client[" << sd << "], because: "
				  << strerror(errno));
		}
	}
}

void Server::sendOk(int sd)
{
	sendMsg(sd, "OK\n");
}

void Server::sendErr(int sd)
{
	sendMsg(sd, "ERR\n");
}

void Server::sendMsg(int sd, const std::string& msg)
{
	int sent_bytes = 0;

	// cast is safe, because sent_bytes is only negative in case of throw
	while(static_cast<unsigned int>(sent_bytes) != msg.size())
	{
		int ret = send(sd, msg.c_str(), msg.size(), 0);

		if(ret == -1)
		{
			DEBUG("MSG: " << msg << " : " << strerror(errno) << errno);

			throw NetworkException(errno);

		}
		else
		{
			sent_bytes += ret;
		}
	}

	DEBUG("Sent bytes: " << msg.size());
}
