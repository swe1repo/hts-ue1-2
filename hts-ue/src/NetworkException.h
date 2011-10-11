/*

    .-..-..-----. .--. .-..-. .--.   ,-.
    : :; :`-. .-': .--': :: :: .--'.'  :
    :    :  : :  `. `. : :: :: `;   `: :
    : :: :  : :   _`, :: :; :: :__   : :
    :_;:_;  :_;  `.__.'`.__.'`.__.'  :_;


    By Benjamin Steindl & Patrick Schwab

    File: NetworkException.h

*/
    :_;:_;  :_;  `.__.'`.__.'`.__.'  :_;


    By Benjamin Steindl & Patrick Schwab

    File: NetworkException.h

*/

#ifndef NETWORKEXCEPTION_H_
#define NETWORKEXCEPTION_H_

#include <exception>
#include <cstring>

class NetworkException : public std::exception
{
public:
	NetworkException() :
		errno_(0)
	{
	}

	NetworkException(int errnum) :
		errno_(errnum)
	{
	}

	~NetworkException() throw()
	{
	}

    virtual const char* what() const throw()
    {
        return strerror(errno_);
    }

	int errno_;
};

#endif /* NETWORKEXCEPTION_H_ */
