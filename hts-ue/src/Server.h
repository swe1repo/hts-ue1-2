/*

    .-..-..-----. .--. .-..-. .--.   ,-.
    : :; :`-. .-': .--': :: :: .--'.'  :
    :    :  : :  `. `. : :: :: `;   `: :
    : :: :  : :   _`, :: :; :: :__   : :
    :_;:_;  :_;  `.__.'`.__.'`.__.'  :_;


    By Benjamin Steindl & Patrick Schwab

    File: Server.h

*/
    :_;:_;  :_;  `.__.'`.__.'`.__.'  :_;


    By Benjamin Steindl & Patrick Schwab

    File: Server.h

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
