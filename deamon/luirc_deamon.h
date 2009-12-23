#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <syslog.h>
#include <signal.h>
#include <getopt.h>
#include <unistd.h>  /* _exit, fork */
#include <errno.h>   /* errno */
#include <map>

#define VERSION_NUMBER "0.2"
#define UPARROW 0
#define DOWNARROW 1
#define LEFTARROW 2
#define RIGHTARROW 3
#define ENTER 4
#define EXIT 5
#define CHUP 6
#define CHDOWN 7

void cleanup(int signal);