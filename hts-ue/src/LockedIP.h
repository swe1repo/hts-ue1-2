/*

    .-..-..-----. .--. .-..-. .--.   ,-.
    : :; :`-. .-': .--': :: :: .--'.'  :
    :    :  : :  `. `. : :: :: `;   `: :
    : :: :  : :   _`, :: :; :: :__   : :
    :_;:_;  :_;  `.__.'`.__.'`.__.'  :_;


    By Benjamin Steindl & Patrick Schwab

    File: LockedIP.h

*/

#ifndef LockedIP_H_
#define LockedIP_H_

#include <time.h>
#include <string>
#include <list>


class LockedIP
{
private:
	static std::list<std::string> ip_addr;

	int attemptCount_;
	time_t timestamp_;
	std::string ip_string_;

	void setTimestamp();
public:
	LockedIP(std::string ip_string);
	bool isLocked();
	void lock();
	void unlock();
	void incrementCount();
	int getCount();
	void setCount(int c);
	time_t getTimestamp();
	void printIP();
};

#endif
