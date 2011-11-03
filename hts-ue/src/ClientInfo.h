/*
 * ClientInfo.h
 *
 *  Created on: Nov 3, 2011
 *      Author: patrick
 */

#ifndef CLIENTINFO_H_
#define CLIENTINFO_H_

#include <arpa/inet.h>
#include <string>
#include <ctime>
#include <boost/thread/mutex.hpp>

class ClientInfo
{
public:
	ClientInfo(struct sockaddr_in client_address);

	std::string getIPAddressString() const;
	void loginFailed();
	void loginSucceeded();
	void lock();
	void unlock();
	bool isLocked();
	time_t getTimestamp();

	bool operator==(const ClientInfo& rh);
private:
	struct sockaddr_in client_address_;
	int attemptCount_;
	time_t timestamp_;
	boost::mutex client_mutex_;

	ClientInfo(const ClientInfo&);
};

#endif /* CLIENTINFO_H_ */
