#include "VideoCapture.hpp"

VideoCapture::InitVideoCapture() {
    LOG_INFO("[VideoCapture] Initializing video capture for source: ", source_);
    // Initialize GStreamer elements here
    // This is a placeholder for actual initialization logic
}

VideoCapture::VideoCapture(const std::string& source)
    : source_(source), running_(false) {}

VideoCapture::~VideoCapture() {
    stop();
}

void VideoCapture::start() {
    running_ = true;
    LOG_INFO("Started capturing from: ", source_);
}

void VideoCapture::stop() {
    if (running_) {
        running_ = false;
        LOG_INFO("[VideoCapture] Stopped capturing.");
    }
}
