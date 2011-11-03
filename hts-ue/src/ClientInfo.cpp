/*
 * ClientInfo.cpp
 *
 *  Created on: Nov 3, 2011
 *      Author: patrick
 */

#include "ClientInfo.h"
#include "ClientRestrictionManager.h"

ClientInfo::ClientInfo(struct sockaddr_in client_address) :
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
	boost::mutex::scoped_lock lock(client_mutex_);

	attemptCount_++;
}

void ClientInfo::loginSucceeded()
{
	boost::mutex::scoped_lock lock(client_mutex_);

	attemptCount_ = 0;
}

void ClientInfo::lock()
{
	boost::mutex::scoped_lock lock(client_mutex_);

	timestamp_ = time(0);
	attemptCount_ = 3;
}

void ClientInfo::unlock()
{
	boost::mutex::scoped_lock lock(client_mutex_);

	attemptCount_ = 0;
}

bool ClientInfo::isLocked()
{
	boost::mutex::scoped_lock lock(client_mutex_);

	return attemptCount_ < 3 ? false : true;
}

time_t ClientInfo::getTimestamp()
{
	boost::mutex::scoped_lock lock(client_mutex_);

	return timestamp_;
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

