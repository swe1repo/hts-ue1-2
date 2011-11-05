/*

    .-..-..-----. .--. .-..-. .--.   ,-.
    : :; :`-. .-': .--': :: :: .--'.'  :
    :    :  : :  `. `. : :: :: `;   `: :
    : :: :  : :   _`, :: :; :: :__   : :
    :_;:_;  :_;  `.__.'`.__.'`.__.'  :_;


    By Benjamin Steindl & Patrick Schwab

    File: FileManager.cpp

*/

#include "FileManager.h"
#include "Logging.h"

namespace fs = boost::filesystem;
namespace ip = boost::interprocess;

FileManager::FileManager() :
	AbstractFileManager()
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

	DEBUG("The mailspooldirectory is at " << directory_path_ << ".");
}

void FileManager::persistSendMessage(const SendMessage& msg)
{
	if(directory_path_.empty())
	{
		throw FileManagerException("Directory has not been set yet.");
	}

	foreach(std::string receiver, msg.receivers_)
	{
		fs::path user_directory = directory_path_ / fs::path(receiver);

		// get exclusive access for the user directory - this lock is scoped
		ip::file_lock dir_lock(user_directory.string().c_str());

		dir_lock.lock();

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
			count_cache_.insert(std::pair<std::string, int>(receiver, 0));
		}

		dir_lock.unlock();

		if( !fs::is_directory(user_directory) )
		{
			throw FileManagerException("Local filesystem conflict: Cannot create user specific directory.");
		}

		if(count_cache_.find(receiver) == count_cache_.end())
		{
			// obtain shared access for reading - this is a scoped lock
			ip::file_lock f_lock(user_directory.string().c_str());
			ip::sharable_lock<ip::file_lock> lock(f_lock);

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

			count_cache_.insert(std::pair<std::string, int>(receiver, count));
		}

		std::string num_msg;

		try
		{
			num_msg = boost::lexical_cast<std::string>(count_cache_[receiver]);
		}
		catch(const boost::bad_lexical_cast& e)
		{
			throw FileManagerException("Failed to convert to string.");
		}

		fs::path msg_file = user_directory / fs::path( num_msg ) ;

		fs::ofstream of;

		// get exclusive access for writing - this lock is scoped
		ip::file_lock f_lock(msg_file.string().c_str());
		ip::scoped_lock<ip::file_lock> lock(f_lock);

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

		count_cache_[receiver]++;
	}
}

std::vector<std::string> FileManager::getMessageList(const ListMessage& msg)
{
	if(directory_path_.empty())
	{
		throw FileManagerException("Directory has not been set yet.");
	}

	std::vector<std::string> ret;

	fs::path user_directory = directory_path_ / fs::path(msg.username_);

	if(!fs::exists(user_directory))
	{
		return ret;
	}

	fs::directory_iterator end;

	// obtain shared access for reading - this is a scoped lock
	ip::file_lock f_lock(user_directory.string().c_str());
	ip::sharable_lock<ip::file_lock> lock(f_lock);

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

	// eclipse parser is stupid, prevents warning
#ifdef __CDT_PARSER__
	return boost::shared_ptr<SendMessage>();
#endif
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
		// get exclusive access for writing - this lock is scoped
		ip::file_lock f_lock(file_path.string().c_str());
		ip::scoped_lock<ip::file_lock> lock(f_lock);

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

boost::shared_ptr<SendMessage> FileManager::messageFromFile(std::string filename)
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
