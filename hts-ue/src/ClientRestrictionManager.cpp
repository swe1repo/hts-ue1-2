/*
 * ClientRestrictionManager.cpp
 *
 *  Created on: Nov 3, 2011
 *      Author: patrick
 */

#include "ClientRestrictionManager.h"
#include "Logging.h"

namespace fs = boost::filesystem;

ClientRestrictionManager::ClientRestrictionManager() :
	lockout_time_((time_t)60) // default lockout_time_ is 60 seconds
{
	try
	{
		locked_clients_ = ThreadedFileManager::getInstance()->loadIpFile( fs::path("iplist") );
	}
	catch(const FileManagerException& e)
	{
		DEBUG(e.what());
		throw e;
	}
}

ClientRestrictionManager::~ClientRestrictionManager()
{
	ThreadedFileManager::getInstance()->saveIpFile( fs::path("iplist"), locked_clients_ );
}

void ClientRestrictionManager::update(time_t deltaT)
{
	boost::unique_lock<boost::shared_mutex> lock(clients_mutex_);

	auto it = locked_clients_.begin();

	for(; it != locked_clients_.end(); it++)
	{
		// lockout time is over
		if( time(0) - it->second > lockout_time_)
		{
			DEBUG("Unlocked IP: " << it->first);

			// remove the client ip from locked list
			locked_clients_.erase(it);
		}
	}
}

void ClientRestrictionManager::setLockoutTime(time_t lockout_time)
{
	lockout_time_ = lockout_time;
}

void ClientRestrictionManager::lock(std::string client_ip)
{
	boost::unique_lock<boost::shared_mutex> lock(clients_mutex_);

	DEBUG("Locking IP: " << client_ip);

	locked_clients_.insert(std::pair<std::string, time_t>(client_ip, time(0)));
}

bool ClientRestrictionManager::isLocked(std::string client_ip)
{
	boost::shared_lock<boost::shared_mutex> lock(clients_mutex_);

	auto it = locked_clients_.begin();

	for(; it != locked_clients_.end(); it++)
	{
		// this client is locked
		if( client_ip.compare(it->first) == 0)
		{
			return true;
		}
	}

	return false;
}

ClientInfo* ClientRestrictionManager::getCurrentClient()
{
	return current_client_.get();
}

void ClientRestrictionManager::setCurrentClient(ClientInfo* ci)
{
	current_client_.reset(ci);
}

