/*

    .-..-..-----. .--. .-..-. .--.   ,-.
    : :; :`-. .-': .--': :: :: .--'.'  :
    :    :  : :  `. `. : :: :: `;   `: :
    : :: :  : :   _`, :: :; :: :__   : :
    :_;:_;  :_;  `.__.'`.__.'`.__.'  :_;


    By Benjamin Steindl & Patrick Schwab

    File: ServerMain.cpp

*/

#include <csignal>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "WelcomeServer.h"
#include "FileManager.h"
#include "Logging.h"

static WelcomeServer* ws = 0;

void signal_handler(int signal)
{
	DEBUG("Caught " << signal << ", aborting after cleanup.");

	if(ws != 0)
	{
		delete ws;
		ws = 0;
	}

	exit(EXIT_FAILURE);
}

void print_usage()
{
	std::cout << "Usage: ./Server [LISTEN_PORT] [DIRECTORY]" << std::endl;
}

int main(int argc, char** argv)
{
	std::string directory_path;
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

		directory_path = argv[2];
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

	FileManager::getInstance()->setDirectoryPath(directory_path);

	ws = new WelcomeServer(port);

	try
	{
		ws->run();
	}
	catch(const NetworkException& e)
	{
		DEBUG(e.what());

		delete ws;
		ws = 0;

		return EXIT_FAILURE;
	}

	delete ws;
	ws = 0;

	return EXIT_SUCCESS;
}


