#pragma once

#include "Logger.hpp"
#include <json/json.h>
#include <string>
#include <functional>
#include <unordered_map>

class RestService {
public:
    RestService();
    ~RestService() = default;

    // Process an incoming REST request and return a JSON response
    std::string processRequest(const std::string& method, const std::string& path, const Json::Value& requestBody);

    // Public method to create an error response (accessible from main.cpp)
    std::string createErrorResponse(const std::string& message);

private:
    using HandlerFunc = std::function<std::string(const Json::Value&)>;
    std::unordered_map<std::string, HandlerFunc> handlers_;

    // Specific endpoint handlers

    std::string handleGetStreamStatus(const Json::Value& request);
    std::string handleSetResolutionFps(const Json::Value& request);
    std::string handleGetResolutionFps(const Json::Value& request);
    std::string handleSetBitrate(const Json::Value& request);
    std::string handleGetBitrate(const Json::Value& request);
    std::string handleSetEncoder(const Json::Value& request);
    std::string handleGetEncoder(const Json::Value& request);
    std::string handleSetFlip(const Json::Value& request);
    std::string handleGetFlip(const Json::Value& request);
    std::string handleSetRotate(const Json::Value& request);
    std::string handleGetRotate(const Json::Value& request);
    std::string handleSetCrop(const Json::Value& request);
    std::string handleGetCrop(const Json::Value& request);
    std::string handleUpgradeFromRemote(const Json::Value& request);
    std::string handleUpgradeFromLocal(const Json::Value& request);
    std::string handleStartRecording(const Json::Value& request);
    std::string handleStopRecording(const Json::Value& request);
    std::string handleGetRecordings(const Json::Value& request);
    std::string handleSetStaticIP(const Json::Value& request);
    std::string handleSetDynamicIP(const Json::Value& request);
    std::string handleGetNetworkDetails(const Json::Value& request);
    std::string handleGetSystemInfo(const Json::Value& request);
    std::string handleAddUserAccount(const Json::Value& request);
    std::string handleDeleteUserAccount(const Json::Value& request);
    std::string handleModifyUserAccount(const Json::Value& request);
    std::string handleVerifyUserAccount(const Json::Value& request);
    std::string handleStartInferencing(const Json::Value& request);
    std::string handleStopInferencing(const Json::Value& request);
    std::string handleStartStream(const Json::Value& request);
    std::string handleStopStream(const Json::Value& request);

    // Utility to create JSON response (remains private)
    std::string createJsonResponse(bool success, const std::string& message, const Json::Value& data = Json::Value());
};