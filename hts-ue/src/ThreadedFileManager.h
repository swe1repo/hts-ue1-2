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
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>

#include "AbstractFileManager.h"
#include "Singleton.h"
#include "FileLockDelegate.h"

class ThreadedFileManager : public AbstractFileManager, Singleton<ThreadedFileManager>
{
public:
	// AbstractFileManager base
	virtual void setDirectoryPath(std::string directory_path);
	virtual void persistSendMessage(const SendMessage& msg);
	virtual std::vector<std::string> getMessageList(const ListMessage& msg);
	virtual boost::shared_ptr<SendMessage> getMessageForRead(const ReadMessage& msg);
	virtual void removeFile(const DelMessage& msg);
private:
	FileLockDelegate lockDelegate_;
	boost::filesystem::path directory_path_;

	ThreadedFileManager();
	boost::filesystem::path accessUserDirectory(std::string username);
	boost::filesystem::path accessMessageDirectory(std::string username, std::string message_id);
	boost::filesystem::path accessDirectory(fs::path path);
	void createDirectory(fs::path path);
	std::string getRandomMessageId();

	friend class Singleton<ThreadedFileManager>;
};

#endif /* THREADEDFILEMANAGER_H_ */
