/*
 * ClientRestrictionManager.h
 *
 *  Created on: Nov 3, 2011
 *      Author: patrick
 */

#ifndef CLIENTRESTRICTIONMANAGER_H_
#define CLIENTRESTRICTIONMANAGER_H_

#include <vector>
#include <string>
#include <boost/thread/mutex.hpp>
#include <boost/thread/tss.hpp>
#include "Singleton.h"
#include "ClientInfo.h"

class ClientRestrictionManager : public Singleton<ClientRestrictionManager>
{
public:
	void setLockoutTime(time_t lockout_time);
	void addClient(ClientInfo* client);
	void removeClient(ClientInfo* client);
	void removeCurrentClient();
	ClientInfo* getCurrentClient();
	bool isLocked(ClientInfo* client);
	void lock(ClientInfo* client);
	void update(time_t deltaT);
private:
	ClientRestrictionManager();

	std::vector<ClientInfo*> clients_;
	time_t lockout_time_;

	boost::thread_specific_ptr<ClientInfo> current_client_;
	boost::mutex clients_mutex_;

	friend class Singleton<ClientRestrictionManager>;
};

#endif /* CLIENTRESTRICTIONMANAGER_H_ */
