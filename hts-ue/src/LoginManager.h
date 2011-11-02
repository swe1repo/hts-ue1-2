/*

    .-..-..-----. .--. .-..-. .--.   ,-.
    : :; :`-. .-': .--': :: :: .--'.'  :
    :    :  : :  `. `. : :: :: `;   `: :
    : :: :  : :   _`, :: :; :: :__   : :
    :_;:_;  :_;  `.__.'`.__.'`.__.'  :_;


    By Benjamin Steindl & Patrick Schwab

    File: LoginManager.h

*/

#ifndef LOGINMANAGER_H_
#define LOGINMANAGER_H_

#include <string>
#include <ldap.h>
#include <list>
#include "Logging.h"
#include "LockedIP.h"


class LoginManager
{
public:
	LoginManager(std::string client_ip);
	bool validLoginCount();
	bool isLoggedIn();
	int searchUID(char uid[20]);
	bool validatePassword(int count_entries,std::string password);
	bool sendLDAPRequest(std::string username, std::string password);

private:
	bool isLoggedIn_;
	std::string myIP_;
	LDAP *ld;
	LDAPMessage *result, *e;
	char *attribs[2];
	char *dn;
    LockedIP client_ip_;
};

#endif /* LOGINMANAGER_H_ */
