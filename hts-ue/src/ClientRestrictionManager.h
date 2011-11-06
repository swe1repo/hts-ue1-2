/*
 * ClientRestrictionManager.h
 *
 *  Created on: Nov 3, 2011
 *      Author: patrick
 */

#ifndef CLIENTRESTRICTIONMANAGER_H_
#define CLIENTRESTRICTIONMANAGER_H_

#include <map>
#include <string>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/tss.hpp>
#include "Singleton.h"
#include "ClientInfo.h"
#include "ThreadedFileManager.h"

class ClientRestrictionManager : public Singleton<ClientRestrictionManager>
{
public:
	~ClientRestrictionManager();

	void setLockoutTime(time_t lockout_time);
	ClientInfo* getCurrentClient();
	void setCurrentClient(ClientInfo* ci);
	bool isLocked(std::string client_ip);
	void lock(std::string client_ip);
	void update(time_t deltaT);
private:
	ClientRestrictionManager();

	std::map<std::string, time_t> locked_clients_;
	time_t lockout_time_;

	boost::thread_specific_ptr<ClientInfo> current_client_;
	boost::shared_mutex clients_mutex_;

	friend class Singleton<ClientRestrictionManager>;
};

#endif /* CLIENTRESTRICTIONMANAGER_H_ */
