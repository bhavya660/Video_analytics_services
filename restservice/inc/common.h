#ifndef __IN_COMMON_H__
#define __IN_COMMON_H__

//---------------------------------------------------------------
// INCLUDE
//---------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <cassert>

//---------------------------------------------------------------
// DEFINE
//---------------------------------------------------------------
#define FALSE 0
#define TRUE 1

//---------------------------------------------------------------
// struct and enums
//---------------------------------------------------------------
typedef enum IN_ERR {
    IN_FAIL = -1,
    IN_SUCCESS = 0
} IN_ERR_e;

#endif
