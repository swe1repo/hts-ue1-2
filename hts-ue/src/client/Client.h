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
#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "../messages/Messages.h"
#include "../NetworkException.h"
#include "../readline.h"
#include "responses/Responses.h"
#include "ResponseParser.h"
#include "UserManager.h"
#include "InvalidEntryException.h"

#define DEFAULT_BUFFER_SIZE 1024

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
	std::vector<std::string> total_data_;
	ResponseParser response_parser_;
	Message::MessageType last_type_;
	bool end_receiving_;
	UserManager userManager_;

	// networking
	void didReceiveResponse(int socket, boost::shared_ptr<Response> response);
	void getResponse();
	bool connectToServer();
	bool testConnection();

	// displaying
	char printMenu();
	char printLoginMenu();
	void transmitMessage(const Message& msg);
	bool presentMainMenu();
	bool presentLoginMenu();
	void presentAttachmentMenu(const SendMessage& sm);

	// utility
	char createMenuWithCollection(const std::vector<std::string>& collection);
	void createAndSendQuitMessage();

	template <typename T>
	void readParam(std::string question_text, T& param)
	{
		std::cout << std::endl << "  " << question_text << std::endl;
		std::cin  >> param;
	}

	template <typename T>
	void readParamHidden(std::string question_text, T& param)
	{
		// use stty to hide console input
		system("stty -echo");
		readParam(question_text, param);
		system("stty echo");

		std::cout << std::endl;
	}

	void readStringUntil(std::string question_text, std::string& param)
	{
		// make sure string is cleared
		param.clear();

		std::cout << std::endl << "  " << question_text << std::endl;

		std::string line;

		// read until user inputs a dot and a newline
		while(true)
		{
			std::getline(std::cin, line);

			if(line.compare(".") == 0)
			{
				break;
			}
			else
			{
				// re-append newlines
				param.append(line + "\n");
			}
		}
	}

	bool readYesNoQuestion(std::string question_text)
	{
		while(true)
		{
			char ctrl2;

			readParam(question_text, ctrl2);

			if(ctrl2 == 'y')
			{
				return true;
			}
			else if(ctrl2 == 'n')
			{
				return false;
			}
			else
			{
				// invalid control character entered
				continue;
			}
		}
	}

	Attachment readAttachment(std::string file_path);
};

#endif /* CLIENT_H_ */
