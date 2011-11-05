/*
 * ThreadedFileManager.cpp
 *
 *  Created on: Nov 4, 2011
 *      Author: patrick
 */

#include "ThreadedFileManager.h"
#include "Logging.h"

namespace fs = boost::filesystem;
namespace ip = boost::interprocess;

ThreadedFileManager::ThreadedFileManager() :
	AbstractFileManager()
{
}

void ThreadedFileManager::setDirectoryPath(std::string directory_path)
{
	if(!directory_path_.empty())
	{
		throw FileManagerException("Directory has already been set to " + directory_path_.string() + ".");
	}
	else
	{
		directory_path_ = directory_path;
	}

	if(!fs::is_directory(directory_path_))		// check existance
	{
		DEBUG("The specified mailspooldirectory [ "
				<< directory_path_.string().c_str()
				<< " ] is not a valid directory.");

		throw FileManagerException("Not a directory.");
	}

	if(access(directory_path_.string().c_str(), R_OK | W_OK) == -1) // check permissions
	{
		DEBUG("You don't have sufficient(rw) permissions for the specified directory [ "
				<< directory_path_.string().c_str()
				<< " ].");

		throw FileManagerException("Missing permissions.");
	}

	DEBUG("The mailspooldirectory is at " << directory_path_ << ".");
}

void ThreadedFileManager::persistSendMessage(const SendMessage& msg)
{
	foreach(std::string receiver, msg.receivers_)
	{
		auto msg_path = accessMessageDirectory(receiver, getRandomMessageId());
	}
}

std::vector<std::string> ThreadedFileManager::getMessageList(const ListMessage& msg)
{
	return std::vector<std::string>();
}

boost::shared_ptr<SendMessage> ThreadedFileManager::getMessageForRead(const ReadMessage& msg)
{
	return boost::shared_ptr<SendMessage>();
}

void ThreadedFileManager::removeFile(const DelMessage& msg)
{

}

fs::path ThreadedFileManager::accessUserDirectory(std::string username)
{
	return accessDirectory( directory_path_ / fs::path(username) );
}

fs::path ThreadedFileManager::accessMessageDirectory(std::string username, std::string message_id)
{
	return accessDirectory( accessDirectory(username) + fs::path(message_id) );
}

fs::path ThreadedFileManager::accessDirectory(fs::path path)
{
	if( !fs::exists(path) )
	{
		createDirectory(path);
	}

	return path;
}

void ThreadedFileManager::createDirectory(fs::path path)
{
	if(mkdir(path.string().c_str(), 0777) == -1)
	{
		throw FileManagerException("Failed to create directory at path " + path.string() + ".");
	}
}

std::string ThreadedFileManager::getRandomMessageId()
{
#define FILENAME_LEN 15

	// seed nrand with previous value
	static unsigned short init[3];
	long int* pInit = reinterpret_cast<long int>(&init[0]);

	std::string retVal( FILENAME_LEN );
	std::vector<long int> randVal( FILENAME_LEN );

	for(int i = 0; i < FILENAME_LEN; ++i)
	{
		randVal.push_back( *pInit = nrand48( init ) );
	}

	int size = sizeof(long int) * FILENAME_LEN;
	char buf[ size ];

	foreach(long int val, randVal)
	{
		snprintf(buf, size, "%x", val);
	}
/*
	if( retVal.compare() == 0 )
	{
		throw FileManagerException("Failed to create another random messageId")
	}
*/
}
