/*
 * Server.h
 *
 *  Created on: Sep 27, 2011
 *      Author: patrick
 */

#ifndef WELCOMESERVER_H_
#define WELCOMESERVER_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

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
    void shutdown();
    void setBufferSize(int bufferSize);
    void setQueueSize(int queueSize);

protected:
    void mainLoop();
    void handleClient(int socket);

private:
    int welcome_socket_;
    int buffer_size_;
    int queue_size_;
    int port_;
    std::vector<int> clients_;
};

#endif /* WELCOMESERVER_H_ */