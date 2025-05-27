#ifndef __RS_COMMON_H__
#define  __RS_COMMON_H__
//-----------------------------------------------------------------------------
// INCLUDE
//-----------------------------------------------------------------------------
#include <bits/stdc++.h>
#include <iostream>
#include <sys/time.h>
#include <queue>
#include <string>
#include <cstring>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <chrono>
#include <functional>
#include <jsoncpp/json/json.h>
#include <condition_variable>
#include <pthread.h>
#include <time.h>
#include <tuple>
#include <map>
//-----------------------------------------------------------------------------
// DEFINE
//-----------------------------------------------------------------------------
#ifndef FALSE
#define FALSE  0
#endif
#ifndef TRUE
#define TRUE   1
#endif

//-----------------------------------------------------------------------------
// TYPEDEF
//-----------------------------------------------------------------------------
using namespace std;
using namespace std::chrono;

/** Status code describing error */
typedef enum
{
    /** Success message for request */
    RS_SUCCESS = 0,
    /** Error message for request */
    RS_ERROR = 1,
}StatusCode;
//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
#endif

