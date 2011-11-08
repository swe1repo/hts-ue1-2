/*
 * ThreadedFileManager.cpp
 *
 *  Created on: Nov 4, 2011
 *      Author: patrick
 */

#include "ThreadedFileManager.h"
#include "Logging.h"

std::string ThreadedFileManager::config_directory_(".config");

namespace fs = boost::filesystem;

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
		const char* cstr = receiver.c_str();
		try
		{
			// exclusive lock on each directory for writing the file
			boost::interprocess::named_mutex nm(boost::interprocess::open_or_create, cstr);
			boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(nm);

			std::string msg_id = MessageIdGenerator::getInstance()->getUniqueMessageId();

			auto usr_path = accessUserDirectory(receiver);

			writeMessageToFile( usr_path / fs::path( msg_id ), msg );
		}
		catch(...)
		{
			boost::interprocess::named_mutex::remove(cstr);

			throw;
		}
	}
}

// walks the directory specified by __msg__ and creates a list of its content msgs' titles
std::vector<std::string> ThreadedFileManager::getMessageList(const ListMessage& msg)
{
	std::vector<std::string> retVal;

	const char* cstr = msg.username_.c_str();
	try
	{
		// exclusive lock on each directory for writing the file
		boost::interprocess::named_mutex nm(boost::interprocess::open_or_create, cstr);
		boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(nm);

		fs::path usr_path = directory_path_ / fs::path(msg.username_);

		if(!fs::is_directory(usr_path))
		{
			throw FileManagerException("User directory at " + usr_path.string() + " doesn't exist.");
		}

		fs::directory_iterator end;
		std::multimap<std::time_t, std::string> sorted_files;

		for(fs::directory_iterator it(usr_path); it != end; it++)
		{
			fs::path msg_file = it->path();

			if( fs::is_regular_file(msg_file) )
			{
				auto pMsg = messageFromFile(msg_file.string());

				auto val = std::pair<std::time_t, std::string>
                	     			(fs::last_write_time( msg_file ), pMsg->title_);

				sorted_files.insert( val );
			}
		}

		auto it2 = sorted_files.begin();

		for(; it2 != sorted_files.end(); it2++)
		{
			// append in sorted order
			retVal.push_back( it2->second );
		}
	}
	catch(...)
	{
		boost::interprocess::named_mutex::remove(cstr);

		throw;
	}

	return retVal;
}

boost::shared_ptr<SendMessage> ThreadedFileManager::getMessageForRead(const ReadMessage& msg)
{
	const char* cstr = msg.username_.c_str();
	try
	{
		// exclusive lock on each directory for writing the file
		boost::interprocess::named_mutex nm(boost::interprocess::open_or_create, cstr);
		boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(nm);

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
	catch(...)
	{
		boost::interprocess::named_mutex::remove(cstr);

		throw;
	}

	// will never happen
	return boost::shared_ptr<SendMessage>();
}

void ThreadedFileManager::removeFile(const DelMessage& msg)
{
	const char* cstr = msg.username_.c_str();
	try
	{
		// exclusive lock on each directory for writing the file
		boost::interprocess::named_mutex nm(boost::interprocess::open_or_create, cstr);
		boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(nm);

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
	catch(...)
	{
		boost::interprocess::named_mutex::remove(cstr);

		throw;
	}
}

fs::path ThreadedFileManager::getMessageAtIndex(std::string username, int index)
{
	fs::path usr_path = directory_path_ / fs::path(username);

	if(!fs::is_directory(usr_path))
	{
		throw FileManagerException("Directory at " + usr_path.string() + " doesn't exist yet.");
	}

	fs::directory_iterator end;
	fs::directory_iterator it(usr_path);
	std::multimap<std::time_t, fs::path> sorted_files;

	for(; it != end; it++)
	{
		fs::path msg_file = it->path();

		if( fs::is_regular_file(msg_file) )
		{
			auto val = std::pair<std::time_t, fs::path>
            	     			(fs::last_write_time( msg_file ), msg_file);

			sorted_files.insert( val );
		}
	}

	auto it2 = sorted_files.begin();

	for(int i = 0; it2 != sorted_files.end(); it2++, i++)
	{
		if( i == index )
		{
			fs::path msg_file = it2->second;

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

	DEBUG("Successfully wrote file " << msg_file.string());

	of.close();
}

// deserializes a message from path __filename__
boost::shared_ptr<SendMessage> ThreadedFileManager::messageFromFile(std::string filename)
{
	try
	{
		std::string file_str = readAll( fs::path(filename) );

		return boost::shared_ptr<SendMessage>( new SendMessage(file_str) );
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

std::map<std::string, time_t> ThreadedFileManager::loadIpFile(boost::filesystem::path path)
{
	fs::path ipfile_path = accessDirectory( directory_path_ /
				 	 	     fs::path( ThreadedFileManager::config_directory_ ) ) /
				             path;

	if(!fs::exists(ipfile_path))
	{
		// first server start - theres no iplist yet
		return std::map<std::string, time_t>();
	}

	std::string file_str = readAll( ipfile_path );

	std::map<std::string, time_t> retVal;
	std::stringstream ss( file_str );

	while( true )
	{
		std::string line;
		std::getline(ss, line, '\n');

		if( ss.eof() )
		{
			break;
		}

		std::vector<std::string> tokens;

		// split on space,
		// tokens are of format: ${IP_ADDRESS} ${UNLOCK_TIME}
		char * pch;
		pch = strtok(const_cast<char*>( line.c_str() ), " ");

		while (pch != NULL)
		{
			tokens.push_back(std::string(pch));
			pch = strtok (NULL, " ");
		}

		time_t conv_time;

		try
		{
			// convert to time_t
			conv_time = boost::lexical_cast<time_t>( tokens[1] );
		}
		catch(const boost::bad_lexical_cast& e)
		{
			DEBUG("IP file is malformed. Discarding. (LEXICAL CAST)");
			return std::map<std::string, time_t>();
		}

		retVal.insert(std::pair<std::string, time_t>(tokens[0], conv_time));
	}

	DEBUG("Loaded locked IP's from " << ipfile_path.string());

	return retVal;
}

void ThreadedFileManager::saveIpFile(boost::filesystem::path path,
									 const std::map<std::string, time_t>& ip_map)
{
	fs::path ipfile_path = accessDirectory( directory_path_ /
				 	 	     fs::path( ThreadedFileManager::config_directory_ ) ) /
				             path;

	fs::ofstream of;

	of.open( ipfile_path );

	auto it = ip_map.begin();

	for(; it != ip_map.end(); it++)
	{
		std::string out;

		out = it->first + " " + boost::lexical_cast<std::string>( it->second ) + "\n";

		of.write(out.c_str(), out.size());
	}

	of.close();

	DEBUG("Dumped locked IP's to " << ipfile_path.string());
}

std::string ThreadedFileManager::readAll(boost::filesystem::path path)
{
	fs::ifstream ifs;

	ifs.open( path );

	if( ifs.fail() )
	{
		throw FileManagerException("Failed to open file. [ " + path.string() + " ]");
	}

	ifs.seekg(0, std::ios::end);

	int len = ifs.tellg();

	ifs.seekg(0, std::ios::beg);

	if(len == -1 || ifs.fail())
	{
		throw FileManagerException("Failed to get file size. [ " + path.string() + " ]");
	}

	std::vector<char> buffer(len);

	ifs.read(&buffer[0], len);

	if(ifs.fail())
	{
		throw FileManagerException("Failed to read file content. [ " + path.string() + " ]");
	}

	ifs.close();

	return std::string(buffer.begin(), buffer.end());
}
