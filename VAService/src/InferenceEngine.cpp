#include "InferenceEngine.hpp"

InferenceEngine::InferenceEngine() {
    LOG_INFO("[InferenceEngine] Initialized.");
}

InferenceEngine::~InferenceEngine() = default;

void InferenceEngine::runInference(const std::string& frame) {
    LOG_INFO("[InferenceEngine] Running inference on frame: " ,frame);
}
