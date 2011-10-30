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
	LoginManager();
	bool validIP(LockedIP lo);
	bool validLoginCount(LockedIP lo);
	void LockClient(LockedIP lo);
	void UnlockClient(LockedIP lo,std::string ip);
	bool isLoggedIn();
	int searchUID(std::string uid);
	bool validatePassword(int count_entries,std::string password);
	void sendLDAPRequest(std::string username, std::string password);

private:
	int rc = 0;		//for checking return value of function calls
	bool isLoggedIn_;
	std::string myIP_;
	LDAP *ld;
	LDAPMessage *result, *e;
	char *attribs[2];
	char *dn;
};

#endif /* LOGINMANAGER_H_ */
