/*
 * UserManager.h
 *
 *  Created on: Oct 25, 2011
 *      Author: patrick
 */

#ifndef USERMANAGER_H_
#define USERMANAGER_H_

#include <string>
#include <vector>

class UserManager
{
public:
	UserManager();
	std::string getUsername();
	bool isLoggedIn();
	void startLogin(std::string username);
	void loginSucceeded();
	void loginFailed();
	void logout();
private:
	bool isLoggedIn_;
	std::string username_;
};

#endif /* USERMANAGER_H_ */
