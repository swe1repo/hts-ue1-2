/*

    .-..-..-----. .--. .-..-. .--.   ,-.
    : :; :`-. .-': .--': :: :: .--'.'  :
    :    :  : :  `. `. : :: :: `;   `: :
    : :: :  : :   _`, :: :; :: :__   : :
    :_;:_;  :_;  `.__.'`.__.'`.__.'  :_;


    By Benjamin Steindl & Patrick Schwab

    File: FileManager.h

*/


#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_

#include <vector>
#include <map>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>
#include "Singleton.h"
#include "AbstractFileManager.h"

class FileManager : public Singleton<FileManager>, public AbstractFileManager
{
	friend class Singleton<FileManager>;

public:
	virtual void setDirectoryPath(std::string directory_path);
	virtual void persistSendMessage(const SendMessage& msg);
	virtual std::vector<std::string> getMessageList(const ListMessage& msg);
	virtual boost::shared_ptr<SendMessage> getMessageForRead(const ReadMessage& msg);
	virtual void removeFile(const DelMessage& msg);

private:
	FileManager();
	boost::filesystem::path directory_path_;
	std::map<std::string, int> count_cache_;

	boost::shared_ptr<SendMessage> messageFromFile(std::string filename);
};

#endif /* FILEMANAGER_H_ */
