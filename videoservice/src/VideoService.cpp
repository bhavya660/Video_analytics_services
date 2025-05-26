/******************************************************************************
 *  File Name   : StreamingService.cpp
 *  Author      : eInfochips
 *******************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// HEADER SECTION
/////////////////////////////////////////////////////////////////////////////
#include "VideoService.h"
namespace fs = std::filesystem;

/////////////////////////////////////////////////////////////////////////////
// GLOBAL SECTION
/////////////////////////////////////////////////////////////////////////////
bool run_app = true;
gboolean terminate_app = FALSE;

/////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
/////////////////////////////////////////////////////////////////////////////
SS_ERR_e HandleResolutionAndFps(StreamingService& ss, int resolution_width, int resolution_height, int fps) {
    
    LOG("Resolution changed to %dx%d with FPS %d", resolution_width, resolution_height, fps);
    SS_ERR_e ret = SS_SUCCESS;
    GstElement *capsfilter = NULL;

    do
    {
        ret = ss.stopPipeline();
        if(ret != SS_SUCCESS)
        {
            LOG("Failed to stop pipeline");
            break;
        }
        else
        {
            LOG("Pipeline stopped successfully");
        }

        capsfilter = gst_bin_get_by_name(GST_BIN(ss.resbin), "capsfilter");
        if (NULL == capsfilter)
        {
            LOG("Failed to create capsfilter element");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("capsfilter element created successfully");
        GstCaps *filtercaps = NULL;
        filtercaps = gst_caps_new_simple("video/x-raw",
                                          "width", G_TYPE_INT, resolution_width,
                                          "height", G_TYPE_INT, resolution_height,
                                          "framerate", GST_TYPE_FRACTION, fps, 1,
                                          NULL);
        LOG("Setting caps for video resolution : %s", gst_caps_to_string(filtercaps));
        g_object_set(G_OBJECT(capsfilter), "caps", filtercaps, NULL);
        gst_caps_unref(filtercaps);

        ret = ss.startPipeline();
        if(ret != SS_SUCCESS)
        {
            LOG("Failed to start pipeline");
            break;
        }
        else
        {
            LOG("Pipeline started successfully");
        }

    } while (FALSE);
    return ret;
}

void monitorFileModification(StreamingService& ss) {
// void monitorFileModification() {
    fs::file_time_type lastWriteTime;

    // Set initial timestamp if the file exists
    if (fs::exists("/dev/shm/rest_request.json")) {
        lastWriteTime = fs::last_write_time("/dev/shm/rest_request.json");
    }

    while (true) {
        if (fs::exists("/dev/shm/rest_request.json")) {
            auto currentWriteTime = fs::last_write_time("/dev/shm/rest_request.json");
            if (currentWriteTime != lastWriteTime) {
                lastWriteTime = currentWriteTime;
                std::cout << "[Monitor] File modified. Reading JSON content:\n";

                std::ifstream file("/dev/shm/rest_request.json", std::ifstream::binary);
                if (file.is_open()) {
                    Json::Value root;
                    Json::CharReaderBuilder builder;
                    std::string errs;

                    if (Json::parseFromStream(builder, file, &root, &errs)) {
                        if (root.isMember("type")) {
                            LOG("type: %s",root["type"].asString().c_str());
                            if(root["type"].asString() == "resolution")
                            {
                                LOG("Received resolution request");
                                HandleResolutionAndFps(ss, root["resolution_width"].asInt(), root["resolution_height"].asInt(), root["fps"].asInt());
                            }
                            else if(root["type"].asString() == "bitrate")
                            {
                                LOG("Received bitrate request");
                            }
                            else
                            {
                                LOG("Invalid request type");
                            }
                        } else {
                            LOG("[Monitor] 'type' field not found in JSON.");
                        }
                    } else {
                        std::cerr << "[Monitor] Failed to parse JSON: " << errs << "\n";
                    }
                    file.close();
                } else {
                    std::cerr << "[Monitor] Failed to open file.\n";
                }
            }
        } else {
            std::cout << "[Monitor] File does not exist. Waiting...\n";
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

SS_ERR_e StreamingService::InitConfig(ConfigManager &cm, string configPath)
{
    if (configPath.empty())
    {
        LOG("Config file path is not valid");
        return SS_FAIL;
    }

    LOG("Config file path: %s", configPath.c_str());

    std::ifstream config_file(configPath);
    if (!config_file)
    {
        LOG("Failed to open config file!");
        return SS_FAIL;
    }

    Json::Value root;
    Json::CharReaderBuilder reader;
    std::string errors;

    if (!Json::parseFromStream(reader, config_file, &root, &errors))
    {
        LOG("Failed to parse JSON: %s", errors.c_str());
        return SS_FAIL;
    }

    // Read Config Values (check if they exist before accessing)
    cm.service_name = root.isMember("service_name") ? root["service_name"].asString() : "default_service";
    cm.architecture = root.get("architecture", "unknown").asString();
    cm.cpu_cores = root.get("cpu_cores", 0).asInt();
    cm.gpu_enabled = root.get("gpu_enabled", false).asBool();
    cm.gpu_memory_mb = root.get("gpu_memory_mb", 0).asInt();

    // Stream Settings
    if (root.isMember("stream_settings"))
    {
        const Json::Value &stream_settings = root["stream_settings"];
        cm.resolution_width = stream_settings.get("resolution_width", 0).asInt();
        cm.resolution_height = stream_settings.get("resolution_height", 0).asInt();
        cm.fps = stream_settings.get("fps", 0).asInt();
        cm.bitrate = stream_settings.get("bitrate", 0).asInt();
        cm.source_type = stream_settings.get("source_type", "unknown").asString();

        if (stream_settings.isMember("source_details"))
        {
            const Json::Value &source_details = stream_settings["source_details"];
            cm.location = source_details.get("location", "none").asString();
        }

        cm.output_stream_type = stream_settings.get("output_stream_type", "unknown").asString();
        cm.output_url = stream_settings.get("output_url", "none").asString();
        cm.video_codec = stream_settings.get("video_codec", "unknown").asString();
    }

    // Network Settings
    if (root.isMember("network_settings"))
    {
        const Json::Value &network_settings = root["network_settings"];
        cm.network_port = network_settings.get("port", 0).asInt();
        cm.bind_address = network_settings.get("bind_address", "none").asString();
        cm.tls_enabled = network_settings.get("tls_enabled", false).asBool();
    }

    // Logging
    if (root.isMember("logging"))
    {
        const Json::Value &logging = root["logging"];
        cm.log_level = logging.get("log_level", "info").asString();
        cm.log_file = logging.get("log_file", "default.log").asString();
    }

    // Hardware Acceleration
    cm.hw_acc = root.get("hw_acceleration", "none").asString();

    // Print Config for Debugging
    LOG("Loaded Config:\n"
        "Service Name: %s\n"
        "Architecture: %s\n"
        "CPU Cores: %d\n"
        "GPU Enabled: %d\n"
        "GPU Memory: %d MB\n"
        "Resolution: %dx%d\n"
        "FPS: %d\n"
        "Bitrate: %d\n"
        "Source Type: %s\n"
        "Camera ID: %s\n"
        "Output Stream Type: %s\n"
        "Output URL: %s\n"
        "Video Codec: %s\n"
        "Port: %d\n"
        "Bind Address: %s\n"
        "Log Level: %s\n"
        "Hardware Acceleration: %s\n",
        cm.service_name.c_str(), cm.architecture.c_str(), cm.cpu_cores,
        cm.gpu_enabled, cm.gpu_memory_mb, cm.resolution_width, cm.resolution_height,
        cm.fps, cm.bitrate, cm.source_type.c_str(), cm.location.c_str(),
        cm.output_stream_type.c_str(), cm.output_url.c_str(), cm.video_codec.c_str(),
        cm.network_port, cm.bind_address.c_str(), cm.log_level.c_str(), cm.hw_acc.c_str());

    return SS_SUCCESS;
}

/* Callback for qtdemux pad-added */
static void on_pad_added(GstElement *element, GstPad *pad, gpointer data)
{
    GstElement *decodebin = GST_ELEMENT(data);
    GstPad *sinkpad = gst_element_get_static_pad(decodebin, "sink");

    if (gst_pad_is_linked(sinkpad))
    {
        g_print("Pad already linked. Ignoring.\n");
        g_object_unref(sinkpad);
        return;
    }

    // Try linking pads
    if (gst_pad_link(pad, sinkpad) == GST_PAD_LINK_OK)
    {
        g_print("Linked qtdemux pad to parser successfully.\n");
    }
    else
    {
        g_print("Failed to link qtdemux pad.\n");
    }

    GST_OBJ_UNREF(sinkpad);
}


SS_ERR_e StreamingService::startPipeline()
{
    SS_ERR_e ret = SS_SUCCESS;
    GstStateChangeReturn state;

    do
    {
        LOG("Chaning the pipeline [%s] to playing state", gst_element_get_name(pipeline));

        // Bus for pipeline messages
        state = gst_element_set_state(srcbin, GST_STATE_PLAYING);
        if (GST_STATE_CHANGE_FAILURE == state)
        {
            LOG("Failed to change srcbin state to play");
            ret = SS_FAIL;
            break;
        }
        else
        {
            LOG("Chnaged [%s] state to play state", gst_element_get_name(srcbin));
        }

        state = gst_element_set_state(resbin, GST_STATE_PLAYING);
        if (GST_STATE_CHANGE_FAILURE == state)
        {
            LOG("Failed to change resbin state to play");
            ret = SS_FAIL;
            break;
        }
        else
        {
            LOG("Chnaged [%s] state to play state", gst_element_get_name(resbin));
        }

        state = gst_element_set_state(sinkbin, GST_STATE_PLAYING);
        if (GST_STATE_CHANGE_FAILURE == state)
        {
            LOG("Failed to change sinkbin state to play");
            ret = SS_FAIL;
            break;
        }
        else
        {
            LOG("Chnaged [%s] state to play state", gst_element_get_name(sinkbin));
        }

        state = gst_element_set_state(pipeline, GST_STATE_PLAYING);
        if (GST_STATE_CHANGE_FAILURE == state)
        {
            LOG("Failed to change pipeline state to play");
            ret = SS_FAIL;
            break;
        }
        else
        {
            LOG("Chnaged [%s] state to play state", gst_element_get_name(pipeline));
        }

    } while (FALSE);

    return ret;
}

SS_ERR_e StreamingService::stopPipeline()
{
    SS_ERR_e ret = SS_SUCCESS;
    GstStateChangeReturn state;

    do
    {
        LOG("Chaning the pipeline [%s] to NULL state", gst_element_get_name(pipeline));

        gst_element_send_event(srcbin, gst_event_new_eos());
        state = gst_element_set_state(srcbin, GST_STATE_NULL);
        if (GST_STATE_CHANGE_FAILURE == state)
        {
            LOG("Failed to change srcbin state to play");
            ret = SS_FAIL;
            break;
        }
        else
        {
            LOG("Chnaged [%s] state to null state", gst_element_get_name(srcbin));
        }

        gst_element_send_event(resbin, gst_event_new_eos());
        state = gst_element_set_state(resbin, GST_STATE_NULL);
        if (GST_STATE_CHANGE_FAILURE == state)
        {
            LOG("Failed to change resbin state to play");
            ret = SS_FAIL;
            break;
        }
        else
        {
            LOG("Chnaged [%s] state to null state", gst_element_get_name(resbin));
        }

        gst_element_send_event(sinkbin, gst_event_new_eos());
        state = gst_element_set_state(sinkbin, GST_STATE_NULL);
        if (GST_STATE_CHANGE_FAILURE == state)
        {
            LOG("Failed to change sinkbin state to play");
            ret = SS_FAIL;
            break;
        }
        else
        {
            LOG("Chnaged [%s] state to null state", gst_element_get_name(sinkbin));
        }

        gst_element_send_event(pipeline, gst_event_new_eos());
        state = gst_element_set_state(pipeline, GST_STATE_NULL);
        if (GST_STATE_CHANGE_FAILURE == state)
        {
            LOG("Failed to change pipeline state to play");
            ret = SS_FAIL;
            break;
        }
        else
        {
            LOG("Chnaged [%s] state to null state", gst_element_get_name(pipeline));
        }
    } while (FALSE);

    return ret;
}

SS_ERR_e StreamingService::createSrcBin(const ConfigManager &config)
{
    SS_ERR_e ret = SS_SUCCESS;
    GstPad *srcpad = NULL, *sinkpad = NULL;
    GstPad *ghost_src_pad = NULL, *ghost_sink_pad = NULL;

    do
    {
        srcbin = gst_bin_new("srcbin");
        if (NULL == srcbin)
        {
            LOG("Failed to create srcbin element");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("srcbin element created successfully");

        src = gst_element_factory_make("filesrc", "src");
        if (NULL == src)
        {
            LOG("Failed to create src element");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("src element created successfully");
        g_object_set(G_OBJECT(src), "location", "/files/sample.mp4", NULL);

        demuxer = gst_element_factory_make("qtdemux", "demuxer");
        if (NULL == demuxer)
        {
            LOG("Failed to create demuxer element");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("demuxer element created successfully");

        queue = gst_element_factory_make("queue", "queue");
        if (NULL == queue)
        {
            LOG("Failed to create queue element");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("queue element created successfully");

        decoder = gst_element_factory_make("decodebin", "decoder");
        if (NULL == decoder)
        {
            LOG("Failed to create decoder element");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("decoder element created successfully");

        queue_src = gst_element_factory_make("queue", "queue_src");
        if (NULL == queue_src)
        {
            LOG("Failed to create queue element");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("queue element created successfully");

        gst_bin_add_many(GST_BIN(srcbin), src, demuxer,queue, decoder, queue_src, NULL);
        if (!gst_element_link(src, demuxer))
        {
            LOG("Failed to link elements in the pipeline");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("Elements linked in the pipeline successfully");
        // Signal to connect demuxer and queue
        g_signal_connect(demuxer, "pad-added", G_CALLBACK(on_pad_added), queue);
        if(!gst_element_link(queue, decoder))
        {
            LOG("Failed to link elements in the pipeline");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("Elements linked in the pipeline successfully");
     
        g_signal_connect(decoder, "pad-added", G_CALLBACK(on_pad_added), queue_src);
        

        // Create ghost pads
        srcpad = gst_element_get_static_pad(queue_src, "src");
        if (NULL == srcpad)
        {
            LOG("Failed to get srcpad from queue element");
            GST_OBJ_UNREF(srcpad);
            ret = SS_FAIL;
            break;
        }
        else
            LOG("Got [%s] pad from queue element", gst_pad_get_name(srcpad));

        /* set ghost pad target for enc link */
        if (!gst_element_add_pad(srcbin, gst_ghost_pad_new_no_target("srcbin_enc", GST_PAD_SRC)))
        {
            LOG("Failed to add ghost pad in source bin");
            ret = SS_FAIL;
            break;
        }
        ghost_src_pad = gst_element_get_static_pad(srcbin, "srcbin_enc");
        if (!ghost_src_pad)
        {
            LOG("failed to static ghost pad srcbin_enc from sourcebin");
            ret = SS_FAIL;
            break;
        }
        gst_ghost_pad_set_target(GST_GHOST_PAD(ghost_src_pad), srcpad);
        GST_OBJ_UNREF(srcpad);
        GST_OBJ_UNREF(ghost_src_pad);

        return ret;
    } while (FALSE);
    
    GST_OBJ_UNREF(srcbin);
    GST_OBJ_UNREF(src);
    GST_OBJ_UNREF(demuxer);
    GST_OBJ_UNREF(queue);
    GST_OBJ_UNREF(decoder);
    GST_OBJ_UNREF(queue_src);
    LOG("Failed to create srcbin");

    return ret;
}

SS_ERR_e StreamingService::createLiveSrcBin(const ConfigManager &config)
{
    SS_ERR_e ret = SS_SUCCESS;
    GstPad *srcpad = NULL, *sinkpad = NULL;
    GstPad *ghost_src_pad = NULL, *ghost_sink_pad = NULL;

    do
    {
        srcbin = gst_bin_new("srcbin");
        if (NULL == srcbin)
        {
            LOG("Failed to create srcbin element");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("srcbin element created successfully");

        src = gst_element_factory_make("v4l2src", "src");
        if (NULL == src)
        {
            LOG("Failed to create src element");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("src element created successfully");
        g_object_set(G_OBJECT(src), "device", "/dev/video0", NULL);

        queue = gst_element_factory_make("queue", "queue");
        if (NULL == queue)
        {
            LOG("Failed to create queue element");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("queue element created successfully");

        gst_bin_add_many(GST_BIN(srcbin), src, queue, NULL);
        if (!gst_element_link(src, queue))
        {
            LOG("Failed to link elements in the pipeline");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("Elements linked in the pipeline successfully");

        // Create ghost pads
        srcpad = gst_element_get_static_pad(queue, "src");
        if (NULL == srcpad)
        {
            LOG("Failed to get srcpad from queue element");
            GST_OBJ_UNREF(srcpad);
            ret = SS_FAIL;
            break;
        }
        else
            LOG("Got [%s] pad from queue element", gst_pad_get_name(srcpad));

        /* set ghost pad target for enc link */
        if (!gst_element_add_pad(srcbin, gst_ghost_pad_new_no_target("srcbin_enc", GST_PAD_SRC)))
        {
            LOG("Failed to add ghost pad in source bin");
            ret = SS_FAIL;
            break;
        }
        ghost_src_pad = gst_element_get_static_pad(srcbin, "srcbin_enc");
        if (!ghost_src_pad)
        {
            LOG("failed to static ghost pad srcbin_enc from sourcebin");
            ret = SS_FAIL;
            break;
        }
        gst_ghost_pad_set_target(GST_GHOST_PAD(ghost_src_pad), srcpad);
        GST_OBJ_UNREF(srcpad);
        GST_OBJ_UNREF(ghost_src_pad);

        return ret;
    } while (FALSE);
    
    GST_OBJ_UNREF(srcbin);
    GST_OBJ_UNREF(src);
    GST_OBJ_UNREF(queue);
    LOG("Failed to create srcbin");

    return ret;
}


SS_ERR_e StreamingService::createResBin(const ConfigManager &config)
{
    SS_ERR_e ret = SS_SUCCESS;
    GstPad *srcpad = NULL, *sinkpad = NULL;
    GstPad *ghost_src_pad = NULL, *ghost_sink_pad = NULL;

    do
    {
        resbin = gst_bin_new("resbin");
        if (NULL == srcbin)
        {
            LOG("Failed to create resbin element");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("resbin element created successfully");

        videoconvert = gst_element_factory_make("videoconvert", "videoconvert");
        if (NULL == videoconvert)
        {
            LOG("Failed to create videoconvert element");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("videoconvert element created successfully");
        videoscale = gst_element_factory_make("videoscale", "videoscale");
        if (NULL == videoscale)
        {
            LOG("Failed to create videoscale element");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("videoscale element created successfully");
        capsfilter = gst_element_factory_make("capsfilter", "capsfilter");
        if (NULL == capsfilter)
        {
            LOG("Failed to create capsfilter element");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("capsfilter element created successfully");
        // GstCaps *filtercaps = NULL;
        // filtercaps = gst_caps_new_simple("video/x-raw",
        //                                   "width", G_TYPE_INT, 1280,
        //                                   "height", G_TYPE_INT, 720,
        //                                   "framerate", GST_TYPE_FRACTION, 30, 1,
        //                                   NULL);
        // LOG("Setting caps for video resolution : %s", gst_caps_to_string(filtercaps));
        // g_object_set(G_OBJECT(capsfilter), "caps", filtercaps, NULL);
        // gst_caps_unref(filtercaps);

        GstCaps *filtercaps = NULL;
        filtercaps = gst_caps_new_simple("video/x-raw",
                                          "width", G_TYPE_INT, config.resolution_width,
                                          "height", G_TYPE_INT, config.resolution_height,
                                          "framerate", GST_TYPE_FRACTION, config.fps, 1,
                                          NULL);
        LOG("Setting caps for video resolution : %s", gst_caps_to_string(filtercaps));
        g_object_set(G_OBJECT(capsfilter), "caps", filtercaps, NULL);
        gst_caps_unref(filtercaps);
        queue_res = gst_element_factory_make("queue", "queue_res");
        if (NULL == queue_res)
        {
            LOG("Failed to create queue element");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("queue element created successfully");

        gst_bin_add_many(GST_BIN(resbin), videoconvert, videoscale, capsfilter, queue_res, NULL);
        if (!gst_element_link_many(videoconvert, videoscale, capsfilter, queue_res, NULL))
        {
            LOG("Failed to link elements in the pipeline");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("Elements linked in the pipeline successfully");

        // Create sink ghost pad
        sinkpad = gst_element_get_static_pad(videoconvert, "sink");
        if (NULL == sinkpad)
        {
            LOG("Failed to get sinkpad from videoconvert element");
            GST_OBJ_UNREF(sinkpad);
            ret = SS_FAIL;
            break;
        }
        else
            LOG("Got [%s] pad from videoconvert element", gst_pad_get_name(sinkpad));
        /* set ghost pad target for enc link */
        if(!gst_element_add_pad(resbin, gst_ghost_pad_new_no_target("resbin_enc_sink", GST_PAD_SINK)))
        {
            LOG("Failed to add ghost pad in source bin");
            ret = SS_FAIL;
            break;
        }
        ghost_sink_pad = gst_element_get_static_pad(resbin, "resbin_enc_sink");
        if (!ghost_sink_pad)
        {
            LOG("failed to static ghost pad resbin_enc_sink from sourcebin");
            ret = SS_FAIL;
            break;
        }
        gst_ghost_pad_set_target(GST_GHOST_PAD(ghost_sink_pad), sinkpad);
        GST_OBJ_UNREF(sinkpad);
        GST_OBJ_UNREF(ghost_sink_pad);

        // Create ghost pads
        srcpad = gst_element_get_static_pad(queue_res, "src");
        if (NULL == srcpad)
        {
            LOG("Failed to get srcpad from queue element");
            GST_OBJ_UNREF(srcpad);
            ret = SS_FAIL;
            break;
        }
        else
            LOG("Got [%s] pad from queue element", gst_pad_get_name(srcpad));

        /* set ghost pad target for enc link */
        if (!gst_element_add_pad(resbin, gst_ghost_pad_new_no_target("resbin_enc_src", GST_PAD_SRC)))
        {
            LOG("Failed to add ghost pad in res bin");
            ret = SS_FAIL;
            break;
        }
        ghost_src_pad = gst_element_get_static_pad(resbin, "resbin_enc_src");
        if (!ghost_src_pad)
        {
            LOG("failed to static ghost pad resbin_enc_src from resbin");
            ret = SS_FAIL;
            break;
        }
        gst_ghost_pad_set_target(GST_GHOST_PAD(ghost_src_pad), srcpad);
        GST_OBJ_UNREF(srcpad);
        GST_OBJ_UNREF(ghost_src_pad);

        return ret;
    } while (FALSE);

    GST_OBJ_UNREF(resbin);
    GST_OBJ_UNREF(videoconvert);
    GST_OBJ_UNREF(videoscale);
    GST_OBJ_UNREF(capsfilter);
    GST_OBJ_UNREF(queue_res);
    

    LOG("Failed to res srcbin");

    return ret;
}

SS_ERR_e StreamingService::createSinkBin(const ConfigManager &config)
{
    SS_ERR_e ret = SS_SUCCESS;
    GstPad *srcpad = NULL, *sinkpad = NULL;
    GstPad *ghost_src_pad = NULL, *ghost_sink_pad = NULL;

    do
    {
        sinkbin = gst_bin_new("sinkbin");
        if (NULL == sinkbin)
        {
            LOG("Failed to create sinkbin element");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("sinkbin element created successfully");

        videoconvert_sink = gst_element_factory_make("videoconvert", "videoconvert_sink");
        if (NULL == videoconvert_sink)
        {
            LOG("Failed to create videoconvert element");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("videoconvert element created successfully");

        encoder = gst_element_factory_make("x264enc", "encoder");
        if (NULL == encoder)
        {
            LOG("Failed to create encoder element");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("encoder element created successfully");
        // g_object_set(G_OBJECT(encoder), "bitrate", 4000, NULL);
        g_object_set(G_OBJECT(encoder), "bitrate", config.bitrate, NULL);
        g_object_set(G_OBJECT(encoder), "speed-preset", "superfast", NULL);

        parser = gst_element_factory_make("h264parse", "parser");
        if (NULL == parser)
        {
            LOG("Failed to create parser element");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("parser element created successfully");
        g_object_set(G_OBJECT(parser), "config-interval", 1, NULL);

        mux = gst_element_factory_make("mpegtsmux", "mux");
        if (NULL == mux)
        {
            LOG("Failed to create mpegtsmux element");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("mpegtsmux element created successfully");

        sink = gst_element_factory_make("udpsink", "sink");
        if (NULL == sink)
        {
            LOG("Failed to create sink element");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("sink element created successfully");
        // g_object_set(G_OBJECT(ss.sink), "location", "/dev/shm/output.h264", NULL);
        g_object_set(G_OBJECT(sink), "host", "172.25.5.196", NULL);
        g_object_set(G_OBJECT(sink), "port", 5000, NULL);
        g_object_set(G_OBJECT(sink), "sync", TRUE, NULL);

        gst_bin_add_many(GST_BIN(sinkbin), videoconvert_sink, encoder, parser, mux, sink, NULL);
        if (!gst_element_link_many(videoconvert_sink, encoder, parser, mux, sink, NULL))
        {
            LOG("Failed to link elements in the pipeline");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("Elements linked in the pipeline successfully");

        // Create sink ghost pad
        sinkpad = gst_element_get_static_pad(videoconvert_sink, "sink");
        if (NULL == sinkpad)
        {
            LOG("Failed to get sinkpad from videoconvert element");
            GST_OBJ_UNREF(sinkpad);
            ret = SS_FAIL;
            break;
        }
        else
            LOG("Got [%s] pad from videoconvert element", gst_pad_get_name(sinkpad));
        /* set ghost pad target for enc link */
        if(!gst_element_add_pad(sinkbin, gst_ghost_pad_new_no_target("sinkbin_enc_sink", GST_PAD_SINK)))
        {
            LOG("Failed to add ghost pad in sink bin");
            ret = SS_FAIL;
            break;
        }
        ghost_sink_pad = gst_element_get_static_pad(sinkbin, "sinkbin_enc_sink");
        if (!ghost_sink_pad)
        {
            LOG("failed to static ghost pad sinkbin_enc_sink from sourcebin");
            ret = SS_FAIL;
            break;
        }
        gst_ghost_pad_set_target(GST_GHOST_PAD(ghost_sink_pad), sinkpad);
        GST_OBJ_UNREF(sinkpad);
        GST_OBJ_UNREF(ghost_sink_pad);

        return ret;
        
    } while (FALSE);
    
    GST_OBJ_UNREF(sinkbin);
    GST_OBJ_UNREF(videoconvert_sink);
    GST_OBJ_UNREF(encoder);
    GST_OBJ_UNREF(sink);
    LOG("Failed to sink srcbin");

    return ret;
}

SS_ERR_e StreamingService::createPipeline(const ConfigManager &config)
{
    SS_ERR_e ret = SS_SUCCESS;
    GstPad *srcpad = nullptr, *sinkpad = nullptr;

    do
    {
        pipeline = gst_pipeline_new("Video_pipeline");
        if (!pipeline)
        {
            LOG("Failed to create gstreamer pipeline");
            ret = SS_FAIL;
            break;
        }
        LOG("Pipeline [%s] created successfully", gst_element_get_name(pipeline));

        LOG("source type : %s", config.source_type.c_str());
        if(config.source_type == "file")
        {
            LOG("Creating srcbin for file source");
            ret = createSrcBin(config);
        }
        else if(config.source_type == "live")
        {
            LOG("Creating srcbin for live source");
            ret = createLiveSrcBin(config);
        }
        else
        {
            LOG("Invalid source type");
            ret = SS_FAIL;
            break;
        }
        ret = createSrcBin(config);
        if (!srcbin)
        {
            LOG("Failed to create srcbin element");
            ret = SS_FAIL;
            break;
        }
        LOG("srcbin element created successfully");

        ret = createResBin(config);
        if (!resbin)
        {
            LOG("Failed to create resbin element");
            ret = SS_FAIL;
            break;
        }
        LOG("resbin element created successfully");

        ret = createSinkBin(config);
        if (!sinkbin)
        {
            LOG("Failed to create sinkbin element");
            ret = SS_FAIL;
            break;
        }
        LOG("sinkbin element created successfully");

        // Add bins to pipeline
        gst_bin_add(GST_BIN(pipeline), srcbin);
        gst_bin_add(GST_BIN(pipeline), resbin);
        gst_bin_add(GST_BIN(pipeline), sinkbin);

        // Link srcbin -> resbin
        srcpad = gst_element_get_static_pad(srcbin, "srcbin_enc");
        if (!srcpad)
        {
            LOG("Failed to get srcpad of sourcebin");
            ret = SS_FAIL;
            break;
        }
        LOG("[%s] got from sourcebin", gst_pad_get_name(srcpad));

        sinkpad = gst_element_get_static_pad(resbin, "resbin_enc_sink");
        if (!sinkpad)
        {
            LOG("Failed to get sinkpad from resbin");
            ret = SS_FAIL;
            GST_OBJ_UNREF(srcpad);
            break;
        }
        LOG("[%s] got from resbin", gst_pad_get_name(sinkpad));

        if (!gst_element_link_pads(srcbin, gst_pad_get_name(srcpad), resbin, gst_pad_get_name(sinkpad)))
        {
            LOG("Failed to link sourcebin and res bin");
            ret = SS_FAIL;
            GST_OBJ_UNREF(srcpad);
            GST_OBJ_UNREF(sinkpad);
            break;
        }
        LOG("Linked source bin and res bin");
        GST_OBJ_UNREF(srcpad);
        GST_OBJ_UNREF(sinkpad);

        // Link resbin -> sinkbin
        srcpad = gst_element_get_static_pad(resbin, "resbin_enc_src");
        if (!srcpad)
        {
            LOG("Failed to get srcpad of resbin");
            ret = SS_FAIL;
            break;
        }
        LOG("[%s] got from resbin", gst_pad_get_name(srcpad));

        sinkpad = gst_element_get_static_pad(sinkbin, "sinkbin_enc_sink");
        if (!sinkpad)
        {
            LOG("Failed to get sinkpad from sinkbin");
            ret = SS_FAIL;
            GST_OBJ_UNREF(srcpad);
            break;
        }
        LOG("[%s] got from sinkbin", gst_pad_get_name(sinkpad));

        if (!gst_element_link_pads(resbin, gst_pad_get_name(srcpad), sinkbin, gst_pad_get_name(sinkpad)))
        {
            LOG("Failed to link resbin and sinkbin");
            ret = SS_FAIL;
            GST_OBJ_UNREF(srcpad);
            GST_OBJ_UNREF(sinkpad);
            break;
        }
        LOG("Linked res bin and sink bin");

        GST_OBJ_UNREF(srcpad);
        GST_OBJ_UNREF(sinkpad);

        GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");

    } while (false);

    return ret;
}


void detectObjects(cv::Mat &frame, cv::dnn::Net &net) {
    cv::Mat blob;
    cv::dnn::blobFromImage(frame, blob, 1/255.0, cv::Size(416, 416), cv::Scalar(), true, false);
    net.setInput(blob);

    std::vector<cv::Mat> outputs;
    net.forward(outputs, net.getUnconnectedOutLayersNames());

    float confidenceThreshold = 0.4;
    std::vector<cv::Rect> boxes;
    std::vector<int> classIds;
    std::vector<float> confidences;
    
    for (const auto &output : outputs) {
        for (int i = 0; i < output.rows; i++) {
            const float *data = output.ptr<float>(i);
            float confidence = data[4];
            if (confidence > confidenceThreshold) {
                int classId = std::max_element(data + 5, data + output.cols) - (data + 5);
                int x = static_cast<int>(data[0] * frame.cols);
                int y = static_cast<int>(data[1] * frame.rows);
                int w = static_cast<int>(data[2] * frame.cols);
                int h = static_cast<int>(data[3] * frame.rows);
                boxes.emplace_back(x - w / 2, y - h / 2, w, h);
                classIds.push_back(classId);
                confidences.push_back(confidence);
            }
        }
    }
    
    for (size_t i = 0; i < boxes.size(); i++) {
        cv::rectangle(frame, boxes[i], cv::Scalar(0, 255, 0), 2);
        // std::string label = "Object " + std::to_string(classIds[i]) + " " + std::to_string(confidences[i]);
        // cv::putText(frame, label, cv::Point(boxes[i].x, boxes[i].y - 10),
        //             cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 2);
    }
}

// static GstPadProbeReturn buffer_probe_callback(GstPad *pad, GstPadProbeInfo *info, gpointer user_data)
// {
//     // LOG("Buffer probe callback");
//     GstBuffer *buffer = GST_PAD_PROBE_INFO_BUFFER(info);
//     if (!buffer)
//         return GST_PAD_PROBE_OK;

//     GstMapInfo map;
//     if (gst_buffer_map(buffer, &map, GST_MAP_READ))
//     {
//         // Convert buffer data to OpenCV Mat
//         cv::Mat yuvFrame(720 * 1.5, 1280, CV_8UC1, (void *)map.data);
//         cv::Mat bgrFrame;
//         cv::cvtColor(yuvFrame, bgrFrame, cv::COLOR_YUV2BGR_NV12);

//         // // Run YOLO object detection
//         cv::dnn::Net *net = static_cast<cv::dnn::Net *>(user_data);
//         detectObjects(bgrFrame, *net); // This will draw bounding boxes on `bgrFrame`

//         // // Convert back to NV12 format for GStreamer pipeline
//         cv::Mat modifiedYUV;
//         cv::cvtColor(bgrFrame, modifiedYUV, cv::COLOR_BGR2YUV_I420);

//         GstBuffer *new_buffer = gst_buffer_new_allocate(NULL, modifiedYUV.total() * modifiedYUV.elemSize(), NULL);
//         GstMapInfo new_map;
//         if (gst_buffer_map(new_buffer, &new_map, GST_MAP_WRITE)) {
//             memcpy(new_map.data, modifiedYUV.data, modifiedYUV.total() * modifiedYUV.elemSize());
//             gst_buffer_unmap(new_buffer, &new_map);
//         }

//         // Copy metadata from original buffer to new buffer
//         gst_buffer_copy_into(new_buffer, buffer,
//             (GstBufferCopyFlags)(GST_BUFFER_COPY_FLAGS | GST_BUFFER_COPY_TIMESTAMPS | GST_BUFFER_COPY_META),
//             0, -1);

//         // Replace the original buffer with the modified buffer
//         GST_PAD_PROBE_INFO_DATA(info) = new_buffer;
//         gst_buffer_unref(buffer); // Unreference the original buffer

//         // // Create a new GstBuffer with modified frame data
//         // GstBuffer *new_buffer = gst_buffer_new_allocate(NULL, modifiedYUV.total() * modifiedYUV.elemSize(), NULL);
//         // GstMapInfo new_map;
//         // if (gst_buffer_map(new_buffer, &new_map, GST_MAP_WRITE)) {
//         //     memcpy(new_map.data, modifiedYUV.data, modifiedYUV.total() * modifiedYUV.elemSize());
//         //     gst_buffer_unmap(new_buffer, &new_map);
//         // }

//         // // Replace the original buffer with the modified buffer
//         // GST_PAD_PROBE_INFO_DATA(info) = new_buffer;
//         // gst_buffer_unref(buffer); // Unreference the original buffer

//         // saveFrame(bgrFrame, frame_count++);
//         // gst_buffer_unmap(buffer, &map);
//     }
//     return GST_PAD_PROBE_OK;
// }

static GstPadProbeReturn buffer_probe_callback(GstPad *pad, GstPadProbeInfo *info, gpointer user_data)
{
    int width = 0, height = 0;

    GstBuffer *buffer = GST_PAD_PROBE_INFO_BUFFER(info);
    if (!buffer || !gst_buffer_is_writable(buffer))
        return GST_PAD_PROBE_OK;

    GstCaps *caps = gst_pad_get_current_caps(pad);
    if (caps)
    {
        GstStructure *structure = gst_caps_get_structure(caps, 0);
        gst_structure_get_int(structure, "width", &width);
        gst_structure_get_int(structure, "height", &height);
        gst_caps_unref(caps);
    }
    
    GstMapInfo map;
    if (gst_buffer_map(buffer, &map, GST_MAP_READWRITE))
    {
        // cv::Mat yuvFrame(720 * 3 / 2, 1280, CV_8UC1, (void *)map.data);
        cv::Mat yuvFrame(height * 3 / 2, width, CV_8UC1, (void *)map.data);
        cv::Mat bgrFrame;
        cv::cvtColor(yuvFrame, bgrFrame, cv::COLOR_YUV2BGR_I420);

        cv::dnn::Net *net = static_cast<cv::dnn::Net *>(user_data);
        detectObjects(bgrFrame, *net); // modifies bgrFrame in-place

        cv::Mat modifiedYUV;
        cv::cvtColor(bgrFrame, modifiedYUV, cv::COLOR_BGR2YUV_I420);

        memcpy(map.data, modifiedYUV.data, modifiedYUV.total() * modifiedYUV.elemSize());

        gst_buffer_unmap(buffer, &map);
    }

    return GST_PAD_PROBE_OK;
}

cv::dnn::Net loadYOLO() {
    std::string modelConfiguration = "yolov3-tiny.cfg";
    std::string modelWeights = "yolov3-tiny.weights";
    cv::dnn::Net net = cv::dnn::readNetFromDarknet(modelConfiguration, modelWeights);
    net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
    net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    return net;
}

void ExitApp(int signum)
{
    LOG("Exit: Signal {%d} received.", signum);

    run_app = false;
    terminate_app = TRUE;
    return;
}

int main()
{
    SS_ERR_e ret = SS_SUCCESS;
    StreamingService ss;
    ConfigManager cm;
    LOG("This is streaming service execution");

    /* Signal handler Initialization */
    signal(SIGINT, ExitApp);
    signal(SIGKILL, ExitApp);
    signal(SIGTERM, ExitApp);

    do
    {
        ret = ss.InitConfig(cm, CONFIG_PATH);
        if (ret != SS_SUCCESS)
        {
            LOG("InitConfig() failed");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("InitConfig() success");

        // Initializing Gstreamer
        gst_init(NULL, NULL);
        cv::dnn::Net net = loadYOLO();

        // std::thread([] {
        //     monitorFileModification();
        // }).detach();
        std::thread([&ss]() {
            monitorFileModification(ss);
        }).detach();
        LOG("Thread created to monitor file");
        
        ret = ss.createPipeline(cm);
        if (ret != SS_SUCCESS)
        {
            LOG("createPipeline() failed");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("createPipeline() success");

        
        GstElement *queue = gst_bin_get_by_name(GST_BIN(ss.resbin), "queue_res");
        GstPad *srcpad = gst_element_get_static_pad(ss.queue_res, "src");
        LOG("Got srcpad from queue_res[%s]", gst_pad_get_name(srcpad));
        gst_pad_add_probe(srcpad, GST_PAD_PROBE_TYPE_BUFFER, buffer_probe_callback, &net, nullptr);

        ret = ss.startPipeline();
        if (ret != SS_SUCCESS)
        {
            LOG("startPipeline() failed");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("startPipeline() success");

        GstBus *bus = gst_element_get_bus(ss.pipeline);

        while (!terminate_app)
        {
            GstMessage *msg = gst_bus_timed_pop_filtered(bus, 100 * GST_MSECOND, GstMessageType(GST_MESSAGE_ANY));

            if (msg != nullptr)
            {
                switch (GST_MESSAGE_TYPE(msg))
                {
                case GST_MESSAGE_ERROR:
                {
                    GError *err;
                    gchar *debug_info;
                    gst_message_parse_error(msg, &err, &debug_info);
                    LOG("Error received: %s\n", err->message);
                    g_error_free(err);
                    g_free(debug_info);
                    terminate_app = TRUE;
                    break;
                }
                case GST_MESSAGE_EOS:
                    LOG("End of stream\n");
                    terminate_app = TRUE;
                    break;
                default:
                    break;
                }
                gst_message_unref(msg);
            }
        }

        gst_object_unref(bus);

        ret = ss.stopPipeline();
        if (ret != SS_SUCCESS)
        {
            LOG("stopPipeline() failed");
            ret = SS_FAIL;
            break;
        }
        else
            LOG("stopPipeline() success");

        std::this_thread::sleep_for(std::chrono::seconds(2));

    } while (FALSE);

    return ret;
}