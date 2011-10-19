/*
 * ClientMain.cpp
 *
 *  Created on: Oct 10, 2011
 *      Author: patrick
 */


#include <csignal>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "Client.h"
#include "../Logging.h"
#include "../NetworkException.h"

static Client* cl = 0;

void signal_handler(int signal)
{
	DEBUG("Caught " << signal << ", aborting after cleanup.");

	if(cl != 0)
	{
		delete cl;
		cl = 0;
	}

	exit(EXIT_FAILURE);
}

void print_usage()
{
	std::cout << "Usage: ./Client [LISTEN_PORT] [IP_ADRESS]" << std::endl;
}

int main(int argc, char** argv)
{
	std::string ip_address;
	int port;

	if(argc != 3)
	{
		print_usage();
		return EXIT_FAILURE;
	}
	else
	{
		try
		{
			port = boost::lexical_cast<int>(argv[1]);
		}
		catch(const boost::bad_lexical_cast& e)
		{
			print_usage();
			return EXIT_FAILURE;
		}

		ip_address = argv[2];
	}


	if( SIG_ERR == signal(SIGINT, signal_handler) )
	{
		DEBUG("Error when registering signal handler, catching an inexistant signal will not work.");
	}

	if( SIG_ERR == signal(SIGTERM, signal_handler) )
	{
		DEBUG("Error when registering signal handler, catching an inexistant signal will not work.");
	}

	if( SIG_ERR == signal(SIGSEGV, signal_handler) )
	{
		DEBUG("Error when registering signal handler, catching an inexistant signal will not work.");
	}

	cl = new Client(port, ip_address);

	try
	{
		cl->run();
	}
	catch(const NetworkException& e)
	{
		if(e.errno_ != 0)
		{
			std::cout << "  " << "Failed to connect to Server at [ " << ip_address << ":" << port << " ], because: "
					  << e.what() << std::endl;

			std::cout << "  " << "Aborting." << std::endl;

			delete cl;
			cl = 0;
			return EXIT_FAILURE;
		}
		else
		{
			std::cout << "  " << "Connection was successfully closed." << std::endl << std::endl;
		}
	}


	delete cl;
	cl = 0;

	return EXIT_SUCCESS;
}

