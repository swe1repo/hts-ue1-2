/*
 * ClientInfo.cpp
 *
 *  Created on: Nov 3, 2011
 *      Author: patrick
 */

#include "ClientInfo.h"
#include "ClientRestrictionManager.h"
#include "Logging.h"

ClientInfo::ClientInfo(int socket_descriptor, struct sockaddr_in client_address) :
	socket_descriptor_(socket_descriptor),
	client_address_(client_address),
	attemptCount_(0)
{
}

std::string ClientInfo::getIPAddressString() const
{
	return std::string(inet_ntoa(client_address_.sin_addr));
}

void ClientInfo::loginFailed()
{
	attemptCount_++;

	if(attemptCount_ >= 3)
	{
		lock();
	}
}

void ClientInfo::loginSucceeded()
{
	attemptCount_ = 0;
}

void ClientInfo::lock()
{
	ClientRestrictionManager::getInstance()->lock(getIPAddressString());
}

bool ClientInfo::isLocked()
{
	return ClientRestrictionManager::getInstance()->isLocked(getIPAddressString());
}

int ClientInfo::getSocketDescriptor()
{
	return socket_descriptor_;
}

bool ClientInfo::operator==(const ClientInfo& rh)
{
	if(this->getIPAddressString().compare( rh.getIPAddressString() ) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

ClientInfo::ClientInfo(const ClientInfo&)
{
	// NOP
}

