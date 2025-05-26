#ifndef __SS_H__
#define __SS_H__

//---------------------------------------------------------------
// INCLUDE
//---------------------------------------------------------------
#include <iostream>
#include <string>
#include <unistd.h>
#include <signal.h>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <atomic>
#include <json/json.h>
#include <gst/gst.h>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

#include "common.h"
#include "logger.h"

using namespace std;

//---------------------------------------------------------------
// DEFINE
//---------------------------------------------------------------
#define CONFIG_PATH "/config/config.json"
#define GST_OBJ_UNREF(obj)     \
	if (obj)                   \
		gst_object_unref(obj); \
	obj = NULL;

//---------------------------------------------------------------
// FUNCTIONS
//---------------------------------------------------------------

class ConfigManager {
    public:
        string service_name;  // Returns singleton instance
        string architecture;
        int cpu_cores;
        bool gpu_enabled;
        int gpu_memory_mb;
        int resolution_width;
        int resolution_height;
        int fps;
        int bitrate;
        string source_type;
        string location;
        string output_stream_type;
        string output_url;
        string video_codec;
        int network_port;
        string bind_address;
        bool tls_enabled;
        string log_level;
        string log_file;
        string hw_acc;
};

class StreamingService {
    public:
        ConfigManager cm;

        GstElement* pipeline = nullptr;

        GstElement* src = nullptr;
        GstElement* demuxer = nullptr;
        GstElement* queue = nullptr;
        GstElement* decoder = nullptr;
        GstElement* queue_src = nullptr;
        
        GstElement* videoconvert = nullptr;
        GstElement *videoscale = nullptr;
        GstElement* capsfilter = nullptr;
        GstElement* queue_res = nullptr;


        GstElement* videoconvert_sink = nullptr;
        GstElement* encoder = nullptr;
        GstElement* parser = nullptr;
        GstElement *mux = nullptr;
        GstElement* sink = nullptr;

        GstElement *srcbin = nullptr;
        GstElement *sinkbin = nullptr;
        GstElement *resbin = nullptr;

        GMainLoop* loop;


        // SS_ERR_e createPipeline(StreamingService &ss);
        SS_ERR_e createPipeline(const ConfigManager &config);
        SS_ERR_e createSrcBin(const ConfigManager &config);
        SS_ERR_e createLiveSrcBin(const ConfigManager &config);
        SS_ERR_e createResBin(const ConfigManager &config);
        SS_ERR_e createSinkBin(const ConfigManager &config);
        SS_ERR_e startPipeline();
        SS_ERR_e stopPipeline();
        SS_ERR_e HandleRequest(const string &request_type);
        SS_ERR_e InitConfig(ConfigManager &cm, string configPath);
        static void onBusMessage(GstBus* bus, GstMessage* message, gpointer user_data);
        void cleanup();
};

#endif