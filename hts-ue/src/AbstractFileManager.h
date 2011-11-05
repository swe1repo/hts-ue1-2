/*
 * AbstractFileManager.h
 *
 *  Created on: Nov 4, 2011
 *      Author: patrick
 */

#ifndef ABSTRACTFILEMANAGER_H_
#define ABSTRACTFILEMANAGER_H_

#include <exception>
#include "messages/Messages.h"

class AbstractFileManager
{
public:
	// must be called prior to usage
	virtual void setDirectoryPath(std::string directory_path) = 0;

	// any of the below methods may throw a FileManagerException
	virtual void persistSendMessage(const SendMessage& msg) = 0;
	virtual std::vector<std::string> getMessageList(const ListMessage& msg) = 0;
	virtual boost::shared_ptr<SendMessage> getMessageForRead(const ReadMessage& msg) = 0;
	virtual void removeFile(const DelMessage& msg) = 0;
protected:
	AbstractFileManager() {;}
	virtual ~AbstractFileManager() {;}
};

class FileManagerException : std::exception
{
public:
	FileManagerException(std::string error_text) :
		error_text_(error_text) {}

	~FileManagerException() throw() {}

    virtual const char* what() const throw()
    {
        return error_text_.c_str();
    }
private:
    std::string error_text_;
};

#endif /* ABSTRACTFILEMANAGER_H_ */
