/*
 * FileManager.cpp
 *
 *  Created on: Oct 2, 2011
 *      Author: patrick
 */

#include "FileManager.h"
#include "Logging.h"

namespace fs = boost::filesystem;

FileManager::FileManager()
{
}

void FileManager::setDirectoryPath(std::string directory_path)
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
}

void FileManager::persistSendMessage(const SendMessage& msg)
{
	if(directory_path_.empty())
	{
		throw FileManagerException("Directory has not been set yet.");
	}

	fs::path user_directory = directory_path_ / fs::path(msg.receiver_);

	if( !fs::exists(user_directory) )
	{
		// user directory doesn't exist yet
		if(mkdir(user_directory.string().c_str(), 0777) == -1)
		{
			DEBUG("Failed to create user specific directory at path [ "
					<< user_directory.string() << " ].");

			throw FileManagerException("Failed to create directory.");
		}

		// no files yet
		count_cache_.insert(std::pair<std::string, int>(msg.receiver_, 0));
	}

	if( !fs::is_directory(user_directory) )
	{
		throw FileManagerException("Local filesystem conflict: Cannot create user specific directory.");
	}

	if(count_cache_.find(msg.receiver_) == count_cache_.end())
	{
		int count = 0;

		fs::directory_iterator end;

		for(fs::directory_iterator it(user_directory); it != end; it++)
		{
			count++;
		}

		DEBUG("counted files for dir [ "
				<< user_directory.string()
				<< " ] .. result: "
				<< count << ".");

		count_cache_.insert(std::pair<std::string, int>(msg.receiver_, count));
	}

	std::string num_msg;

	try
	{
		num_msg = boost::lexical_cast<std::string>(count_cache_[msg.receiver_]);
	}
	catch(const boost::bad_lexical_cast& e)
	{
		throw FileManagerException("Failed to convert to string.");
	}

	fs::path msg_file = user_directory / fs::path( num_msg ) ;

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

	DEBUG("Successfully wrote file " << msg_file.string());

	of.close();

	count_cache_[msg.receiver_]++;
}

std::vector<std::string> FileManager::getMessageList(const ListMessage& msg)
{
	if(directory_path_.empty())
	{
		throw FileManagerException("Directory has not been set yet.");
	}

	int init_count = 5;

	if(count_cache_.find(msg.username_) != count_cache_.end())
	{
		init_count = count_cache_[msg.username_];
	}

	std::vector<std::string> ret;

	fs::path user_directory = directory_path_ / fs::path(msg.username_);

	if(!fs::exists(user_directory))
	{
		return ret;
	}

	fs::directory_iterator end;

	for(fs::directory_iterator it(user_directory); it != end; it++)
	{
		fs::path msg_file = it->path();

		if( fs::is_regular_file(msg_file) )
		{
			auto pMsg = messageFromFile(msg_file.string());

			ret.push_back(pMsg->title_);
		}
	}

	return ret;
}

boost::shared_ptr<SendMessage> FileManager::getMessageForRead(const ReadMessage& msg)
{
	if(directory_path_.empty())
	{
		throw FileManagerException("Directory has not been set yet.");
	}

	std::string index_string;

	try
	{
		index_string = boost::lexical_cast<std::string>(msg.message_number_);
	}
	catch(const boost::bad_lexical_cast& e)
	{
		throw FileManagerException("Index cant be converted to string.");
	}

	fs::path file_path = directory_path_ / fs::path(msg.username_) / fs::path(index_string);

	try
	{
		return messageFromFile( file_path.string() );
	}
	catch(const FileManagerException& e)
	{
		throw FileManagerException("File can't be converted/opened.");
	}
}

void FileManager::removeFile(const DelMessage& msg)
{
	if(directory_path_.empty())
	{
		throw FileManagerException("Directory has not been set yet.");
	}

	std::string index_string;

	try
	{
		index_string = boost::lexical_cast<std::string>(msg.message_number_);
	}
	catch(const boost::bad_lexical_cast& e)
	{
		throw FileManagerException("Index cant be converted to string.");
	}

	fs::path file_path = directory_path_ / fs::path(msg.username_) / fs::path(index_string);

	try
	{
		fs::remove(file_path);
	}
	catch(const fs::filesystem_error& e)
	{
		throw FileManagerException("Failed to remove file at "+ file_path.string() + ", because " + e.what());
	}
}

boost::shared_ptr<SendMessage> FileManager::messageFromFile(std::string filename)
{
	fs::ifstream ifs;

	fs::path msg_file(filename);

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
}
