/*

    .-..-..-----. .--. .-..-. .--.   ,-.
    : :; :`-. .-': .--': :: :: .--'.'  :
    :    :  : :  `. `. : :: :: `;   `: :
    : :: :  : :   _`, :: :; :: :__   : :
    :_;:_;  :_;  `.__.'`.__.'`.__.'  :_;


    By Benjamin Steindl & Patrick Schwab

    File: readline.h

*/

#ifndef READLINE_H_
#define READLINE_H_
#include "Logging.h"
#include <unistd.h>

ssize_t my_read (int fd, char *ptr);
ssize_t readline (int fd, void *vptr, ssize_t maxlen);
ssize_t readn (int fd, void *vptr, size_t n);

#endif /* READLINE_H_ */
