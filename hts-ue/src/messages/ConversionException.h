/*
 * ConversionException.h
 *
 *  Created on: Sep 29, 2011
 *      Author: patrick
 */

#ifndef CONVERSIONEXCEPTION_H_
#define CONVERSIONEXCEPTION_H_

#include <exception>
#include <string>

class ConversionException : public std::exception
{
public:
	ConversionException(const std::string& error_text) :
		error_text_(error_text)
	{
	}

	~ConversionException() throw()
	{
	}

    virtual const char* what() const throw()
    {
        return error_text_.c_str();
    }
private:
    std::string error_text_;
};

#endif /* CONVERSIONEXCEPTION_H_ */
