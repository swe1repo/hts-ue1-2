/*
 * UserManager.cpp
 *
 *  Created on: Oct 25, 2011
 *      Author: patrick
 */

#include "UserManager.h"

UserManager::UserManager() :
	isLoggedIn_(false),
	username_()
{
}

bool UserManager::isLoggedIn()
{
	return isLoggedIn_;
}

std::string UserManager::getUsername()
{
	return username_;
}

void UserManager::startLogin(std::string username)
{
	username_ = username;
}

void UserManager::loginSucceeded()
{
	isLoggedIn_ = true;
}

void UserManager::loginFailed()
{
	logout();
}

void UserManager::logout()
{
	isLoggedIn_ = false;
	username_ = "";
}
