/*
 * Client.h
 *
 *  Created on: Oct 10, 2011
 *      Author: patrick
 */

#ifndef CLIENT_H_
#define CLIENT_H_

#include <iostream>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "../messages/Messages.h"
#include "../NetworkException.h"

class Client
{
public:
	Client(int port, std::string ip_address);
	virtual ~Client();

	void run();
private:
	std::string ip_address_;
	int port_;
	int socket_;

	bool connectToServer();
	char printMenu();
	void transmitMessage(const Message& msg);

	template <typename T>
	void readParam(std::string question_text, T& param)
	{
		std::cout << std::endl << "  " << question_text << std::endl;
		std::cin >> param;
	}

	void readStringUntil(std::string question_text, std::string param, char delimiter)
	{
		std::cout << std::endl << "  " << question_text << std::endl;
		std::getline(std::cin, param, delimiter);
	}
};

#endif /* CLIENT_H_ */
