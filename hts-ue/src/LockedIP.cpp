/*

    .-..-..-----. .--. .-..-. .--.   ,-.
    : :; :`-. .-': .--': :: :: .--'.'  :
    :    :  : :  `. `. : :: :: `;   `: :
    : :: :  : :   _`, :: :; :: :__   : :
    :_;:_;  :_;  `.__.'`.__.'`.__.'  :_;


    By Benjamin Steindl & Patrick Schwab

    File: LockedIP.cpp

*/

#include "LockedIP.h"

LockedIP::LockedIP()
{
	attemptCount = 0;
}

void LockedIP::setIP(std::string str)
{
	ip_addr.pushback(str);
}

void LockedIP::setCount(int c)
{
	attemptCount = c;
}

void LockedIP::incrementCount()
{
	attemptCount++;
}

void LockedIP::setTimestamp()
{
	time(&timestamp);
}

int LockedIP::getCount()
{
	return attemptCount;
}

char LockedIP::getIP()
{
	return ip_addr;
}

time_t LockedIP::getTimestamp()
{
	return timestamp;
}

void LockedIP::deleteIP(std::string ip)
{
	for(int i=0;i<ip_addr.size();i++)
	{
		if(ip.compare(ip_addr[i]) == 0)
		{
			ip_addr.remove(i);
		}
	}
}
