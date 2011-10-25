/*
 * LoginManager.cpp
 *
 *  Created on: Oct 25, 2011
 *      Author: patrick
 */

#include "LoginManager.h"

LoginManager::LoginManager() :
	isLoggedIn_(false)
{

}

bool LoginManager::isLoggedIn()
{
	return isLoggedIn_;
}

void LoginManager::searchUID(std::string uid)
{

}

void LoginManager::sendLDAPRequest(std::string username, std::string password)
{

}
