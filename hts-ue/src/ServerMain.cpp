/*
 * ServerMain.cpp
 *
 *  Created on: Sep 27, 2011
 *      Author: patrick
 */

#include <csignal>
#include <sys/wait.h>
#include <unistd.h>
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

int main(int argc, char** argv)
{
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

	FileManager::getInstance()->setDirectoryPath("/home/patrick/server");

	ws = new WelcomeServer(6006);

	ws->run();

	delete ws;
	ws = 0;

	return EXIT_SUCCESS;
}


