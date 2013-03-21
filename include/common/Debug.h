#ifndef DEBUG_H
#define DEBUG_H
#include <iostream>
#include <unistd.h>
#include <typeinfo> // For printing out type names

using namespace std;

/**
   Colors for debugging purposses
*/
#define RED_BOLD       "\033[1;31m"
#define RED_NORMAL     "\033[0;31m"
#define GREEN_NORMAL   "\033[0;32m"
#define GREEN_BOLD     "\033[1;32m"
#define YELLOW_NORMAL  "\033[0;33m"
#define YELLOW_BOLD    "\033[1;33m"
#define BLUE_NORMAL    "\033[0;34m"
#define BLUE_BOLD      "\033[1;34m"
#define MAGENTA_NORMAL "\033[0;35m"
#define MAGENTA_BOLD   "\033[1;35m"
#define CYAN_NORMAL    "\033[0;36m"
#define CYAN_BOLD      "\033[1;36m"
#define END_COLOR      "\033[0m"
/**
   Macros for debugging purposes
*/

#define SEPARATOR cout << "-------------------------------------------------------------------------------------------------------" << endl;

#define COUT cout << "[" << getpid () << "] "
#define ENDL END_COLOR << endl;

#define REDCOUT COUT << RED_NORMAL
#define REDBCOUT COUT << RED_BOLD

#define GREENCOUT COUT << GREEN_NORMAL
#define GREENBCOUT COUT << GREEN_BOLD

#define YELLOWCOUT COUT << YELLOW_NORMAL
#define YELLOWBCOUT COUT << YELLOW_BOLD

#define BLUECOUT COUT << BLUE_NORMAL
#define BLUEBCOUT COUT << BLUE_BOLD

#define MAGENTACOUT COUT << MAGENTA_NORMAL
#define MAGENTABCOUT COUT << MAGENTA_BOLD

#define CYANCOUT COUT << CYAN_NORMAL
#define CYANBCOUT COUT << CYAN_BOLD


#endif
