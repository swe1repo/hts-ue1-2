/*

    .-..-..-----. .--. .-..-. .--.   ,-.
    : :; :`-. .-': .--': :: :: .--'.'  :
    :    :  : :  `. `. : :: :: `;   `: :
    : :: :  : :   _`, :: :; :: :__   : :
    :_;:_;  :_;  `.__.'`.__.'`.__.'  :_;


    By Benjamin Steindl & Patrick Schwab

    File: FileManager.h

*/
    :_;:_;  :_;  `.__.'`.__.'`.__.'  :_;


    By Benjamin Steindl & Patrick Schwab

    File: FileManager.h

*/

#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_

#include <vector>
#include <map>
#include <exception>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "messages/Messages.h"
#include "Singleton.h"

class FileManager : public Singleton<FileManager>
{
	friend class Singleton<FileManager>;

public:
	void setDirectoryPath(std::string directory_path);
	void persistSendMessage(const SendMessage& msg);
	std::vector<std::string> getMessageList(const ListMessage& msg);
	boost::shared_ptr<SendMessage> getMessageForRead(const ReadMessage& msg);
	void removeFile(const DelMessage& msg);
private:
	FileManager();
	boost::filesystem::path directory_path_;
	std::map<std::string, int> count_cache_;

	boost::shared_ptr<SendMessage> messageFromFile(std::string filename);
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

#endif /* FILEMANAGER_H_ */
