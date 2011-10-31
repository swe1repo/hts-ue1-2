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
	rc(0),
	isLoggedIn_(false)
{

}

/*Lock Client and set timestamp in order to check
 * how long the client is going to be locked
 */
void LoginManager::LockClient(LockedIP lo)
{
	lo.setTimestamp();		//from now on this IP is locked
	//TODO: implement Client in order to wait a specified time until IP gets unlocked
	lo.setIP(myIP_);
	DEBUG("This Client is now locked!");
}

// currently not used
void LoginManager::UnlockClient(LockedIP lo, std::string ip)
{
	lo.deleteIP(ip);
}

/*determine if the IP is on the blacklist
 *
 */
bool LoginManager::validIP(LockedIP lo)
{
	myIP_ = "127.0.0.1"; 		//TODO: set Actual IP from Client

	for(int i=0;i<lo.ip_addr.size();i++)
	{
		if(lo.ip_addr[i].compare(myIP_) == 0)
		{
			DEBUG("Client's IP is locked! No Permission to Connect!");
			return false;
		}
	}
	DEBUG("Client may connect");

	return true;
}

/*check if client has had to many login attempts
 * return true if client may proceed to LDAP connect
 */
bool LoginManager::validLoginCount(LockedIP lo)
{
	if(lo.getCount() < 4)
	{
		DEBUG("Client has permission to connect. Doing LDAP request...");
		return true;
	}
	else
	{
		LockClient();
		return false;
	}
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

	if( (rc = ldap_search_s(ld,SEARCHBASE,SCOPE,uid,attribs,0,&result))
			!= LDAP_SUCCESS)
	{
		DEBUG("LDAP search error: " << ldap_err2string(rc));
		return EXIT_FAILURE;
	}

	int count_entries = 0;
	count_entries = ldap_count_entries(ld,result);
	DEBUG("LDAP search returned " << count_entries << "entries");

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
		if( (rc = ldap_simple_bind_s(ld,dn,password.c_str())) != LDAP_SUCCESS)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}

/*main method of LDAP request
 * makes anonymous bind and creates uid-string for user login
 */
void LoginManager::sendLDAPRequest(std::string username, std::string password)
{
	int rc = 0; 	//for checking return value of function calls
	LockedIP lo = new LockedIP();

	if(validIP(lo) && validLoginCount(lo))
	{
		//do LDAP request
		if( (ld = ldap_init(LDAP_HOST, LDAP_PORT)) == NULL)
		{
			DEBUG("ldap_init failed");
		}
		else
		{
			DEBUG("Connected to LDAP Server " << LDAP_HOST <<" on Port " << LDAP_PORT);
			if( (rc = ldap_simple_bind_s(ld,ANONYM_USER,ANONYM_PW)) != LDAP_SUCCESS)
			{
				DEBUG("LDAP Error at anonymous binding process. " <<ldap_err2string(rc));
				return EXIT_FAILURE;
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
				lo.incrementCount();
				//TODO: tell client that user name is incorrect
			}
			else
			{
				if(validatePassword(count_entries,password))
				{
					DEBUG("Password is correct!");
					lo.setCount(0);		//restore the count of attempts to log in
					//LDAP Process finished at this point

					isLoggedIn_ = true;
				}
				else
				{
					DEBUG("Password is not correct!");
					lo.incrementCount();
					isLoggedIn_ = false;
					//TODO: tell client password is not correct


				}
			}
			free(attribs[0]);
		}
	}
}
