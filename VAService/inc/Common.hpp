#ifndef __VA_COMMON_H__
#define  __VA_COMMON_H__
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
#include <glib.h>
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

#define SEC_TO_MS 1000
#define MS_TO_US  1000
#define SEC_TO_US (SEC_TO_MS)*(MS_TO_US)

/** assert supplied object, free the memory and assign NULL */
#define DELETE_OBJ(obj) if (obj) delete obj; \
                        obj = NULL;

#define GST_OBJ_UNREF(obj) if (obj) gst_object_unref(obj); \
                           obj = NULL;
#ifdef TEST_ENV
#define CONFIG_FILE_PATH "../../config/streaming_config.json"   
#define CAMERA_CONFIGURATION_PATH "../../config/camera_configuration.json"
#else
#define CONFIG_FILE_PATH "/config/streaming_config.json"   
#define CAMERA_CONFIGURATION_PATH "/streaming-data/camera_configuration_"
#define REST_REQUEST_PATH "/streaming-data/rest_request_"
#endif
//-----------------------------------------------------------------------------
// TYPEDEF
//-----------------------------------------------------------------------------
using namespace std;
using namespace std::chrono;

/** Status code describing error */
typedef enum
{
  /** Error occured during message handler setup */
  VA_MSG_HDLER_FAILED = -5,
  /** Error occured during http server setup */
  VA_HTTP_SERVER_FAILED = -4,
  /** Error occured during video source setup */
  VA_VIDEO_SRC_FAILED = -3,
  /** Error occured while allocating memory */
  VA_MEM_ALLOC_FAIL = -2,
  /** General module failure response */
  VA_FAIL = -1,
  /** General module success response  */
  VA_SUCCESS = 0,
  /** Video source already exists */
  VA_VIDEO_SRC_EXISTS = 1,
  /** Video source is not present  */
  VA_VIDEO_SRC_NOT_EXISTS = 2,
  /** Inference branch is invalid */
  VA_VIDEO_SRC_BRANCH_INVALID = 3,
  /** Inference branch is not present */
  VA_VIDEO_SRC_BRANCH_NOT_EXISTS = 4
}StatusCode;
//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
#endif

