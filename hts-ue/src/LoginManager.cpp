/*

    .-..-..-----. .--. .-..-. .--.   ,-.
    : :; :`-. .-': .--': :: :: .--'.'  :
    :    :  : :  `. `. : :: :: `;   `: :
    : :: :  : :   _`, :: :; :: :__   : :
    :_;:_;  :_;  `.__.'`.__.'`.__.'  :_;


    By Benjamin Steindl & Patrick Schwab

    File: LoginManager.cpp

*/

#include "LoginManager.h"

#define LDAP_HOST "ldap.technikum-wien.at"
#define LDAP_PORT 389
#define ANONYM_USER NULL
#define ANONYM_PW NULL
#define SEARCHBASE "dc=technikum-wien,dc=at"
#define SCOPE LDAP_SCOPE_SUBTREE
#define WAIT 60		//specified time to wait after 3 failed logins

LoginManager::LoginManager() :
	isLoggedIn_(false)
{
}

bool LoginManager::isLoggedIn()
{
	return isLoggedIn_;
}

/*search in ldap searchbase for the user
 * returns number of entries for this search (should be one)
 */
int LoginManager::searchUID(char uid[20])
{
	attribs[0] = strdup("uid");
	attribs[1] = NULL;

	int error_code;
	if( (error_code = ldap_search_s(ld, SEARCHBASE, SCOPE, uid, attribs, 0, &result))
	    != LDAP_SUCCESS)
	{
		DEBUG("LDAP search error: " << ldap_err2string(error_code));
	}

	int count_entries = 0;
	count_entries = ldap_count_entries(ld,result);
	DEBUG("LDAP search returned " << count_entries << " entries");

	if(count_entries == 0)
	{
		DEBUG("No entries were found.");
		return 0;
	}

	int x = 0;
	for(e = ldap_first_entry(ld,result);e != NULL;e = ldap_next_entry(ld,e))
	{
		x++;
		dn = ldap_get_dn(ld,e);
		DEBUG("Found Entry " << x << ": " << dn);
		//z.B.: "uid=if10b005,ou=People,dc=technikum-wien,dc=at
	}

	return count_entries;
}

/*Confirm the correctness of the password from the specified user
 * returns true if LDAP bind with password and user id was correct
 */
bool LoginManager::validatePassword(int count_entries,std::string password)
{
	if(count_entries == 1)		//LDAP search should only return one user
	{
		if( ldap_simple_bind_s(ld, dn, password.c_str()) != LDAP_SUCCESS)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		DEBUG("ERROR: LDAP returned more than one user on pw query.");
		return false;
	}
}

/*main method of LDAP request
 * makes anonymous bind and creates uid-string for user login
 */
bool LoginManager::sendLDAPRequest(std::string username, std::string password)
{
	ClientInfo* current_client = ClientRestrictionManager::getInstance()->getCurrentClient();

	if(current_client->isLocked() == false)
	{
		//do LDAP request
		if( (ld = ldap_init(LDAP_HOST, LDAP_PORT)) == NULL)
		{
			DEBUG("ldap_init failed");
			return false;
		}
		else
		{
			int error_code;

			DEBUG("Connected to LDAP Server " << LDAP_HOST <<" on Port " << LDAP_PORT);
			if( (error_code = ldap_simple_bind_s(ld,ANONYM_USER,ANONYM_PW)) != LDAP_SUCCESS)
			{
				DEBUG("LDAP Error at anonymous binding process. " << ldap_err2string(error_code));
			}
			else
			{
				DEBUG("Anonymous Bind Successful!");
			}

			char filter[20];

			strcpy(filter,"(uid=");
			strcat(filter,username.c_str());
			strcat(filter,")");
			DEBUG("Now searching for: " << filter);

			int count_entries;
			count_entries = searchUID(filter);

			if(count_entries == 0)
			{
				DEBUG("User not found!");
				current_client->loginFailed();

				return false;
			}
			else
			{
				if(validatePassword(count_entries,password))
				{
					DEBUG("Password is correct!");
					current_client->loginSucceeded();

					//LDAP Process finished at this point
					isLoggedIn_ = true;

					return true;
				}
				else
				{
					DEBUG("Password is not correct!");
					current_client->loginFailed();
					isLoggedIn_ = false;

					return false;
				}
			}

			// TODO: Move this somewhere it actually has an effect
			free(attribs[0]);
		}
	}

	return false;
}
