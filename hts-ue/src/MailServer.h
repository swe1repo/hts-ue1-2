/*

    .-..-..-----. .--. .-..-. .--.   ,-.
    : :; :`-. .-': .--': :: :: .--'.'  :
    :    :  : :  `. `. : :: :: `;   `: :
    : :: :  : :   _`, :: :; :: :__   : :
    :_;:_;  :_;  `.__.'`.__.'`.__.'  :_;


    By Benjamin Steindl & Patrick Schwab

    File: MailServer.h

*/

#ifndef MAILSERVER_H_
#define MAILSERVER_H_

#include <sys/stat.h>
#include <map>
#include "Server.h"
#include "MessageParser.h"
#include "AbstractFileManager.h"
#include "LoginManager.h"

class MailServer : public Server
{
public:
	MailServer(int sd);

	// has to be called prior to usage
	void setAbstractFileManager(AbstractFileManager* instance);
protected:
    // ServerBase Methods
    void clientConnected();
    void clientReceivedData(boost::shared_ptr<std::string> data);
    void clientDisconnected();
    void clientConnectionTerminated();

    bool didFinishMessage();
private:
    AbstractFileManager* fileManager_;
    MessageParser parser_;
    int socket_id_;
    LoginManager loginManager_;

    void messageReceived(int sd, boost::shared_ptr<Message> msg);

    void handleSend (const SendMessage& msg);
    void handleList (const ListMessage& msg);
    void handleRead (const ReadMessage& msg);
    void handleDel  (const DelMessage& msg);
    void handleQuit (const QuitMessage& msg);
    void handleLogin(const LoginMessage& msg);

    int getAwaitingSize();

    friend class WelcomeServer;
};

#endif /* MAILSERVER_H_ */
