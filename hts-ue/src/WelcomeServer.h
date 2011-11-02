/*

    .-..-..-----. .--. .-..-. .--.   ,-.
    : :; :`-. .-': .--': :: :: .--'.'  :
    :    :  : :  `. `. : :: :: `;   `: :
    : :: :  : :   _`, :: :; :: :__   : :
    :_;:_;  :_;  `.__.'`.__.'`.__.'  :_;


    By Benjamin Steindl & Patrick Schwab

    File: WelcomeServer.h

*/

#ifndef WELCOMESERVER_H_
#define WELCOMESERVER_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "messages/Messages.h"
#include "Server.h"
#include "readline.h"
#include "foreach.h"
#include "Logging.h"
#include "MailServer.h"
#include "FileManager.h"
#include "NetworkException.h"

#define DEFAULT_BUFFER_SIZE 1024
#define DEFAULT_QUEUE_SIZE  5

class WelcomeServer : public Server
{
public:
	WelcomeServer(int port);
	virtual ~WelcomeServer();
    void run();
    void shut_down();
    void setBufferSize(int bufferSize);
    void setQueueSize(int queueSize);

protected:
    void mainLoop();
    void handleClient(int socket, std::string client_ip);

private:
    int welcome_socket_;
    int buffer_size_;
    int queue_size_;
    int port_;
    boost::thread_group clients_;
    std::vector<int> client_sockets_;
};

#endif /* WELCOMESERVER_H_ */
