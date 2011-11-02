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
#include "Logging.h"

std::list<std::string> LockedIP::ip_addr;

LockedIP::LockedIP(std::string ip_string) :
	attemptCount_(0),
	ip_string_(ip_string)
{
}

void LockedIP::printIP()
{
	std::cout << "LockedIP(" << this << ") 's IP address is " << ip_string_ << std::endl;
}

bool LockedIP::isLocked()
{
	return attemptCount_ == 3 ? true : false;
}

void LockedIP::lock()
{
	setTimestamp();
	LockedIP::ip_addr.push_back(ip_string_);
}

void LockedIP::unlock()
{
	auto it = LockedIP::ip_addr.begin();

	for(;it != LockedIP::ip_addr.end(); ++it)
	{
		if(ip_string_.compare(*it) == 0)
		{
			ip_addr.erase(it);
			attemptCount_ = 0;
			return;
		}
	}

	DEBUG("The server tried to unlock a IP address [ " <<
		  ip_string_ <<
		  " ] that wasnt locked.");
}

void LockedIP::setCount(int c)
{
	attemptCount_ = c;
}

void LockedIP::incrementCount()
{
	attemptCount_++;
}

void LockedIP::setTimestamp()
{
	time(&timestamp_);
}

int LockedIP::getCount()
{
	return attemptCount_;
}

time_t LockedIP::getTimestamp()
{
	return timestamp_;
}
