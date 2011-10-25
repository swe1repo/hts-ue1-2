/*
 * InvalidEntryException.h
 *
 *  Created on: Oct 25, 2011
 *      Author: patrick
 */

#ifndef INVALIDENTRYEXCEPTION_H_
#define INVALIDENTRYEXCEPTION_H_

#include <string>
#include <exception>

class InvalidEntryException: public boost::exception
{
public:
	InvalidEntryException(const std::string& error_text) :
		error_text_(error_text)
	{
	}

	~InvalidEntryException() throw()
	{
	}

    virtual const char* what() const throw()
    {
        return error_text_.c_str();
    }
private:
    std::string error_text_;
};

#endif /* INVALIDENTRYEXCEPTION_H_ */
