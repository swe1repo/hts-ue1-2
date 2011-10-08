/*
 * NetworkException.h
 *
 *  Created on: Sep 27, 2011
 *      Author: patrick
 */

#ifndef NETWORKEXCEPTION_H_
#define NETWORKEXCEPTION_H_

#include <exception>
#include <cstring>

class NetworkException : public std::exception
{
public:
	NetworkException()
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
private:
	int errno_;
};

#endif /* NETWORKEXCEPTION_H_ */
