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
		std::string msg_id = MessageIdGenerator::getInstance()->getUniqueMessageId();

		auto usr_path = accessUserDirectory(receiver);

		writeMessageToFile( usr_path / fs::path( msg_id ), msg );
	}
}

// walks the directory specified by __msg__ and creates a list of its content msgs' titles
std::vector<std::string> ThreadedFileManager::getMessageList(const ListMessage& msg)
{
	std::vector<std::string> retVal;

	fs::path usr_path = directory_path_ / fs::path(msg.username_);

	fs::directory_iterator end;

	for(fs::directory_iterator it(usr_path); it != end; it++)
	{
		fs::path msg_file = it->path();

		if( fs::is_regular_file(msg_file) )
		{
			auto pMsg = messageFromFile(msg_file.string());

			retVal.push_back( pMsg->title_ );
		}
	}

	return retVal;
}

boost::shared_ptr<SendMessage> ThreadedFileManager::getMessageForRead(const ReadMessage& msg)
{
	fs::path path = getMessageAtIndex(msg.username_, msg.message_number_);

	if(path.empty())
	{
		throw FileManagerException("File at specified index [" + boost::lexical_cast<std::string>(msg.message_number_) + "] \
	                    			for path " + path.string() + " doesn't exist.");
	}
	else
	{
		return messageFromFile( path.string() );
	}
}

void ThreadedFileManager::removeFile(const DelMessage& msg)
{
	fs::path file_path = getMessageAtIndex(msg.username_, msg.message_number_);

	try
	{
		if(fs::remove(file_path) == false)
		{
			throw FileManagerException("Failed to remove file at "+ file_path.string() + ", because file does not exist.");
		}
	}
	catch(const fs::filesystem_error& e)
	{
		throw FileManagerException("Failed to remove file at "+ file_path.string() + ", because " + e.what());
	}
}

fs::path ThreadedFileManager::getMessageAtIndex(std::string username, int index)
{
	fs::path usr_path = directory_path_ / fs::path(username);

	fs::directory_iterator end;
	fs::directory_iterator it(usr_path);

	for(int i = 0; it != end; i++, it++)
	{
		if( i == index )
		{
			fs::path msg_file = it->path();

			if( fs::is_regular_file(msg_file) )
			{
				return msg_file;
			}
		}
	}

	return fs::path();
}

fs::path ThreadedFileManager::accessUserDirectory(std::string username)
{
	return accessDirectory( directory_path_ / fs::path(username) );
}

fs::path ThreadedFileManager::accessDirectory(fs::path path)
{
	// create directory if it doesn't exist yet
	if( !fs::exists(path) )
	{
		createDirectory(path);
	}

	return path;
}

void ThreadedFileManager::createDirectory(fs::path path)
{
	if( mkdir(path.string().c_str(), 0777) == -1 )
	{
		throw FileManagerException("Failed to create directory at path " + path.string() + ".");
	}
}

// serializes a message __msg__ to file at path __msg_file__
void ThreadedFileManager::writeMessageToFile(fs::path msg_file, const Message& msg)
{
	fs::ofstream of;

	of.open( msg_file );

	if( of.fail() )
	{
		throw FileManagerException("Failed to open file. [ " + msg_file.string() + " ]");
	}

	boost::shared_ptr<std::string> str;

	try
	{
		str = msg.deflate();
	}
	catch(const ConversionException& e)
	{
		throw FileManagerException("Failed to deflate message to file. [ " + msg_file.string() + " ], " + e.what());
	}

	of.write( str->c_str(), str->size() + 1 );

	if(of.fail())
	{
		throw FileManagerException("Failed to write to file. [ " + msg_file.string() + " ]");
	}

	DEBUG("Successfully wrote file " << msg_file.string() << ", content: " << *str);

	of.close();
}

// deserializes a message from path __filename__
boost::shared_ptr<SendMessage> ThreadedFileManager::messageFromFile(std::string filename)
{
	fs::ifstream ifs;

	fs::path msg_file(filename);

	// obtain shared access for reading - this is a scoped lock
	ip::file_lock f_lock(msg_file.string().c_str());
	ip::sharable_lock<ip::file_lock> lock(f_lock);

	ifs.open( msg_file );

	if( ifs.fail() )
	{
		throw FileManagerException("Failed to open file. [ " + msg_file.string() + " ]");
	}

	ifs.seekg(0, std::ios::end);

	int len = ifs.tellg();

	ifs.seekg(0, std::ios::beg);

	if(len == -1 || ifs.fail())
	{
		throw FileManagerException("Failed to get file size. [ " + msg_file.string() + " ]");
	}

	std::vector<char> buffer(len);

	ifs.read(&buffer[0], len);

	if(ifs.fail())
	{
		throw FileManagerException("Failed to read file content. [ " + msg_file.string() + " ]");
	}

	ifs.close();

	try
	{
		return boost::shared_ptr<SendMessage>(new SendMessage(std::string(buffer.begin(), buffer.end())));
	}
	catch(const ConversionException& e)
	{
		throw FileManagerException("Failed to convert " + filename + " to message." + e.what());
	}

	// eclipse parser is stupid, prevents warning
#ifdef __CDT_PARSER__
	return boost::shared_ptr<SendMessage>();
#endif
}
