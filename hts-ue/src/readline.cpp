/*

    .-..-..-----. .--. .-..-. .--.   ,-.
    : :; :`-. .-': .--': :: :: .--'.'  :
    :    :  : :  `. `. : :: :: `;   `: :
    : :: :  : :   _`, :: :; :: :__   : :
    :_;:_;  :_;  `.__.'`.__.'`.__.'  :_;


    By Benjamin Steindl & Patrick Schwab

    File: readline.cpp

*/

#include "readline.h"
#include <errno.h>

#define MAXLINE 1500

static ssize_t
my_read (int fd, char *ptr)
{
 static int   read_cnt = 0 ;
 static char  *read_ptr ;
 static char  read_buf[MAXLINE] ;
 if (read_cnt <= 0) {
   again:
     if ( (read_cnt = read(fd,read_buf,sizeof(read_buf))) < 0) {
       if (errno == EINTR)
         goto again ;
       return (-1) ;
     } else if (read_cnt == 0)
         return (0) ;
     read_ptr = read_buf ;
 } ;
 read_cnt-- ;
 *ptr = *read_ptr++ ;
 return (1) ;
}

ssize_t readline (int fd, void *vptr, ssize_t maxlen)
{
 ssize_t   n, rc ;
 char     c, *ptr ;

 ptr = static_cast<char*>(vptr);

 for (n = 1 ; n < maxlen ; n++) {
   if ( (rc = my_read(fd,&c)) == 1 ) {
     *ptr++ = c ;
     if (c == '\n')
       break ;                // newline is stored
   } else if (rc == 0) {
       if (n == 1)
         return (0) ;         // EOF, no data read
       else
         break ;              // EOF, some data was read
   } else
       return (-1) ;          // error, errno set by read() in my_read()
 } ;
 *ptr = 0 ;                   // null terminate
 return (n) ;
}

