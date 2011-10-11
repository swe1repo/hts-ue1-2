/*
 * Server.cpp
 *
 *  Created on: Oct 4, 2011
 *      Author: patrick
 */

#include "Server.h"
#include "Logging.h"

Server::Server()
{
	// TODO Auto-generated constructor stub

}

void Server::closeSocket(int sd)
{
	if(close(sd) == -1)
	{
		DEBUG("Server failed to close client[" << sd << "], because: "
				<< strerror(errno));
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
			DEBUG(strerror(errno));
			throw NetworkException(errno);
		}
		else
		{
			sent_bytes += ret;
		}
	}

	DEBUG("Sent: " << msg << ", bytes: " << msg.size());

	for(unsigned int i = 0; i < msg.size(); i++)
	{
		DEBUG(static_cast<int>((msg.data())[i]));
	}
}
