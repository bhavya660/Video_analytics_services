#pragma once

#include <iostream>
#include <memory>
#include "Logger.hpp"

class InferenceEngine {
public:
    InferenceEngine();
    ~InferenceEngine();

    SS_ERR_e runInference(const std::string& frame);
};
