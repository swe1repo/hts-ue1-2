/*
 * LoginManager.h
 *
 *  Created on: Oct 25, 2011
 *      Author: patrick
 */

#ifndef LOGINMANAGER_H_
#define LOGINMANAGER_H_

#include <string>
#include <ldap.h>


class LoginManager
{
public:
	LoginManager();
	bool isLoggedIn();
	void searchUID(std::string uid);
	void sendLDAPRequest(std::string username, std::string password);

private:
	bool isLoggedIn_;
};

#endif /* LOGINMANAGER_H_ */
