#ifndef __SS_COMMON_H__
#define __SS_COMMON_H__

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
typedef enum SS_ERR {
    SS_FAIL = -1,
    SS_SUCCESS = 0
} SS_ERR_e;

#endif
