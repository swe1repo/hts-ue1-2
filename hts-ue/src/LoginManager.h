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
#include "ClientRestrictionManager.h"
#include "ClientInfo.h"
#include "Logging.h"


class LoginManager
{
public:
	LoginManager();
	bool isLoggedIn();
	int searchUID(char uid[20]);
	bool validatePassword(int count_entries,std::string password);
	bool sendLDAPRequest(std::string username, std::string password);

private:
	bool isLoggedIn_;
	LDAP *ld;
	LDAPMessage *result, *e;
	char *attribs[2];
	char *dn;
};

#endif /* LOGINMANAGER_H_ */
