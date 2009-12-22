#ifndef preferences_H_
#define preferences_H_
/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 *			Includes
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
//#include <termios.h>
#include <fcntl.h>
#include <syslog.h>
#include <signal.h>
#include <getopt.h>
#include <unistd.h>  /* _exit, fork */
#include <errno.h>   /* errno */

/*
 *		Defines
 */

#define VERSION_NUMBER "0.1"
#define UPARROW 0
#define DOWNARROW 1
#define LEFTARROW 2
#define RIGHTARROW 3
#define ENTER 4
#define EXIT 5
#define CHUP 6
#define CHDOWN 7

/*
 *				Function Defs
 */

void getPreferences(void);


#endif
