/*
 * ThreadedFileManager.h
 *
 *  Created on: Nov 4, 2011
 *      Author: patrick
 */

#ifndef THREADEDFILEMANAGER_H_
#define THREADEDFILEMANAGER_H_

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cstring>
#include <vector>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include "AbstractFileManager.h"
#include "Singleton.h"
#include "MessageIdGenerator.h"

class ThreadedFileManager : public AbstractFileManager, public Singleton<ThreadedFileManager>
{
public:
	// AbstractFileManager base
	virtual void setDirectoryPath(std::string directory_path);

	// message handling
	virtual void persistSendMessage(const SendMessage& msg);
	virtual std::vector<std::string> getMessageList(const ListMessage& msg);
	virtual boost::shared_ptr<SendMessage> getMessageForRead(const ReadMessage& msg);
	virtual void removeFile(const DelMessage& msg);

	// util
	std::string getRandomMessageId();

	// ClientRestrictionManager .config/ipfile
	std::map<std::string, time_t> loadIpFile(boost::filesystem::path path);
	void saveIpFile(boost::filesystem::path path, const std::map<std::string, time_t>& ip_map);
private:
	static std::string config_directory_;
	boost::filesystem::path directory_path_;

	ThreadedFileManager();

	// filesystem handling
	void createDirectory(boost::filesystem::path path);
	boost::filesystem::path accessUserDirectory(std::string username);
	boost::filesystem::path accessDirectory(boost::filesystem::path path);
	std::string readAll(boost::filesystem::path path);

	// utility
	void writeMessageToFile(boost::filesystem::path msg_file, const Message& msg);
	boost::shared_ptr<SendMessage> messageFromFile(std::string filename);
	boost::filesystem::path getMessageAtIndex(std::string username, int index);

	// threadsafe
	void persistOneSendMessage(const SendMessage& msg);

	friend class Singleton<ThreadedFileManager>;
};

#endif /* THREADEDFILEMANAGER_H_ */
