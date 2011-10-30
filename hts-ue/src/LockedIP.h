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
	int attemptCount;
	static List<std::string> ip_addr;
	time_t timestamp;

public:
	LockedIP();
	void incrementCount();
	int getCount();
	void setCount(int c);
	void setIP(std::string str);
	char getIP();
	void deleteIP(std::string ip);
	void setTimestamp();
	time_t getTimestamp();

};

#endif
