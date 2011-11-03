/*
 * ClientRestrictionManager.cpp
 *
 *  Created on: Nov 3, 2011
 *      Author: patrick
 */

#include "ClientRestrictionManager.h"

ClientRestrictionManager::ClientRestrictionManager() :
	lockout_time_((time_t)60) // default lockout_time_ is 60 seconds
{
	// TODO: Load Persistence File
}

void ClientRestrictionManager::update(time_t deltaT)
{
	boost::mutex::scoped_lock lock(clients_mutex_);

	auto it = clients_.begin();

	for(; it != clients_.end(); it++)
	{
		// lockout time is over
		if( time(0) - (*it)->getTimestamp() > lockout_time_)
		{
			(*it)->unlock();
		}
	}
}

void ClientRestrictionManager::setLockoutTime(time_t lockout_time)
{
	lockout_time_ = lockout_time;
}

void ClientRestrictionManager::addClient(ClientInfo* client)
{
	boost::mutex::scoped_lock lock(clients_mutex_);

	clients_.push_back(client);
	current_client_.reset(client);
}

void ClientRestrictionManager::removeClient(ClientInfo* client)
{
	boost::mutex::scoped_lock lock(clients_mutex_);

	auto it = clients_.begin();

	for(; it != clients_.end(); it++)
	{
		// client to remove has been found
		if( *client == *(*it))
		{
			clients_.erase(it);
			current_client_.release();
			return;
		}
	}
}

void ClientRestrictionManager::lock(ClientInfo* client)
{
	boost::mutex::scoped_lock lock(clients_mutex_);

	auto it = clients_.begin();

	for(; it != clients_.end(); it++)
	{
		// client to lock has been found
		if( *client == *(*it))
		{
			(*it)->lock();
			return;
		}
	}
}

ClientInfo* ClientRestrictionManager::getCurrentClient()
{
	return current_client_.get();
}

void ClientRestrictionManager::removeCurrentClient()
{
	removeClient(current_client_.get());
}
