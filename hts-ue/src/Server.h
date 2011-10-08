/*
 * Server.h
 *
 *  Created on: Oct 4, 2011
 *      Author: patrick
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <string>
#include <cstring>
#include <cerrno>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "NetworkException.h"

class Server
{
public:
	Server();

protected:
    void closeSocket(int sd);
    void sendMsg(int sd, const std::string& msg);
    void sendOk(int sd);
    void sendErr(int sd);
};

#endif /* SERVER_H_ */
