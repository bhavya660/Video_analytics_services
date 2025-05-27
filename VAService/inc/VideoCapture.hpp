#pragma once

#include <string>
#include <iostream>
#include <gst/gst.h>
#include "Logger.hpp"

class VideoCapture {
public:
    explicit VideoCapture(const std::string& source);
    ~VideoCapture();
    
    GstElement *pipeline;
    GstElement *srcBin;
    GstElement *EncBin;
    GstElement *InferBin;

    SS_ERR_e InitVideoCapture();
    SS_ERR_e InitInference();
    SS_ERR_e CreatePipeline();
    SS_ERR_e CreateSrcBin();
    SS_ERR_e CreateEncBin();
    SS_ERR_e CreateInferBin();
    SS_ERR_e StartPipeline();
    SS_ERR_e StopPipeline();

private:
    bool running_;
};
