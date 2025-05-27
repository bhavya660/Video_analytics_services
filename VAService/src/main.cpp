#include "VideoCapture.hpp"
#include "InferenceEngine.hpp"
#include "Logger.hpp"

int main() {
    LOG_INFO("[Service] Starting microservice...");

    VideoCapture video("test.mp4");
    InferenceEngine infer;

    video.start();

    // Simulate video processing loop
    for (int i = 0; i < 3; ++i) {
        std::string dummy_frame = "frame_" + std::to_string(i);
        infer.runInference(dummy_frame);
    }

    video.stop();

    LOG_INFO("[Service] Shutting down.");
    return 0;
}
