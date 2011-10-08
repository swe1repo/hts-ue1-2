/*
 * readline.h
 *
 *  Created on: Sep 27, 2011
 *      Author: patrick
 */

#ifndef READLINE_H_
#define READLINE_H_

#include <unistd.h>

ssize_t readline (int fd, void *vptr, ssize_t maxlen);

#endif /* READLINE_H_ */
