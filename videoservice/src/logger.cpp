/******************************************************************************
 *  File Name   : logger.cpp
 *  Author      : eInfochips
 *******************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// HEADER SECTION
/////////////////////////////////////////////////////////////////////////////
#include "logger.h"

/////////////////////////////////////////////////////////////////////////////
// GLOBALS
/////////////////////////////////////////////////////////////////////////////
using namespace std;


/////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
/////////////////////////////////////////////////////////////////////////////
/**
 * @brief logger function
 *
 * @return int
 */
void log_message(const char *func_name, int line, const char *fmt, ...)
{
    time_t now = time(NULL);
    struct tm *local_time = localtime(&now);

    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local_time);

    cout << "["<< time_str <<"] ["<< func_name <<"] ["<< line<<"] ";

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    cout<<endl;
}