#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/config.hpp>
#include <boost/program_options/detail/config_file.hpp>
#include <boost/program_options/parsers.hpp>
#include <fstream>
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

#define DEFAULT_OPTION_FILE "luirc.conf"

//Custom includes
#include "preferences.hpp"

void cleanup(int signal);