#include "RestService.hpp"
#include <json/json.h>

RestService::RestService() {
    // Register endpoint handlers
    handlers_["/get_stream_status"] = [this](const Json::Value& request) { return handleGetStreamStatus(request); };
    handlers_["/set_resolution_fps"] = [this](const Json::Value& request) { return handleSetResolutionFps(request); };
    handlers_["/get_resolution_fps"] = [this](const Json::Value& request) { return handleGetResolutionFps(request); };
    handlers_["/set_bitrate"] = [this](const Json::Value& request) { return handleSetBitrate(request); };
    handlers_["/get_bitrate"] = [this](const Json::Value& request) { return handleGetBitrate(request); };
    handlers_["/set_encoder"] = [this](const Json::Value& request) { return handleSetEncoder(request); };
    handlers_["/get_encoder"] = [this](const Json::Value& request) { return handleGetEncoder(request); };
    handlers_["/set_flip"] = [this](const Json::Value& request) { return handleSetFlip(request); };
    handlers_["/get_flip"] = [this](const Json::Value& request) { return handleGetFlip(request); };
    handlers_["/set_rotate"] = [this](const Json::Value& request) { return handleSetRotate(request); };
    handlers_["/get_rotate"] = [this](const Json::Value& request) { return handleGetRotate(request); };
    handlers_["/set_crop"] = [this](const Json::Value& request) { return handleSetCrop(request); };
    handlers_["/get_crop"] = [this](const Json::Value& request) { return handleGetCrop(request); };
    handlers_["/upgrade_from_remote"] = [this](const Json::Value& request) { return handleUpgradeFromRemote(request); };
    handlers_["/upgrade_from_local"] = [this](const Json::Value& request) { return handleUpgradeFromLocal(request); };
    handlers_["/start_recording"] = [this](const Json::Value& request) { return handleStartRecording(request); };
    handlers_["/stop_recording"] = [this](const Json::Value& request) { return handleStopRecording(request); };
    handlers_["/get_recordings"] = [this](const Json::Value& request) { return handleGetRecordings(request); };
    handlers_["/set_static_ip"] = [this](const Json::Value& request) { return handleSetStaticIP(request); };
    handlers_["/set_dynamic_ip"] = [this](const Json::Value& request) { return handleSetDynamicIP(request); };
    handlers_["/get_network_details"] = [this](const Json::Value& request) { return handleGetNetworkDetails(request); };
    handlers_["/get_system_info"] = [this](const Json::Value& request) { return handleGetSystemInfo(request); };
    handlers_["/add_user_acc"] = [this](const Json::Value& request) { return handleAddUserAccount(request); };
    handlers_["/delete_user_acc"] = [this](const Json::Value& request) { return handleDeleteUserAccount(request); };
    handlers_["/modify_user_acc"] = [this](const Json::Value& request) { return handleModifyUserAccount(request); };
    handlers_["/verify_user_acc"] = [this](const Json::Value& request) { return handleVerifyUserAccount(request); };
    handlers_["/start_inferencing"] = [this](const Json::Value& request) { return handleStartInferencing(request); };
    handlers_["/stop_inferencing"] = [this](const Json::Value& request) { return handleStopInferencing(request); };
    handlers_["/start_stream"] = [this](const Json::Value& request) { return handleStartStream(request); };
    handlers_["/stop_stream"] = [this](const Json::Value& request) { return handleStopStream(request); };
}

std::string RestService::processRequest(const std::string& method, const std::string& path, const Json::Value& requestBody) {
    LOG_INFO("Processing request: ", method, " ", path);

    if (method != "POST") {
        LOG_WARNING("Unsupported HTTP method: ", method);
        return createJsonResponse(false, "Method not allowed");
    }

    auto it = handlers_.find(path);
    if (it != handlers_.end()) {
        return it->second(requestBody);
    }

    LOG_WARNING("Unknown endpoint: ", path);
    return createJsonResponse(false, "Endpoint not found");
}

std::string RestService::createErrorResponse(const std::string& message) {
    return createJsonResponse(false, message);
}

std::string RestService::createJsonResponse(bool success, const std::string& message, const Json::Value& data) {
    Json::Value response;
    response["success"] = success;
    response["message"] = message;
    if (!data.isNull()) {
        response["data"] = data;
    }

    Json::StreamWriterBuilder writer;
    return Json::writeString(writer, response);
}

std::string RestService::handleStartStream(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Starting stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleGetResolutionFps(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Resolution for stream id : ", streamId, " is 1920x1080 at 30fps");
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleSetResolutionFps(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Starting stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleGetStreamStatus(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Starting stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleSetBitrate(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Starting stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleGetBitrate(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Starting stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleSetEncoder(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Starting stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleGetEncoder(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Starting stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleSetFlip(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Starting stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleGetFlip(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Starting stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleSetRotate(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Starting stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleGetRotate(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Starting stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleSetCrop(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Starting stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleGetCrop(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Starting stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleUpgradeFromRemote(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Starting stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleUpgradeFromLocal(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Starting stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleStartRecording(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Starting stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleStopRecording(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Starting stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleGetRecordings(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Starting stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleSetStaticIP(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Starting stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleSetDynamicIP(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Starting stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleGetNetworkDetails(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Starting stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleGetSystemInfo(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Starting stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleAddUserAccount(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Starting stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleDeleteUserAccount(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Starting stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleModifyUserAccount(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Starting stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleVerifyUserAccount(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Starting stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleStartInferencing(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Starting stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleStopInferencing(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Starting stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "started";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream started successfully", responseData);
}

std::string RestService::handleStopStream(const Json::Value& request) {
    if (!request.isMember("stream_id") || !request["stream_id"].isString()) {
        LOG_ERROR("Invalid request: missing or invalid stream_id");
        return createJsonResponse(false, "Invalid request: stream_id required");
    }

    std::string streamId = request["stream_id"].asString();
    LOG_INFO("Stopping stream with ID: ", streamId);
    Json::Value responseData;
    responseData["stream_id"] = streamId;
    responseData["status"] = "stopped";
    responseData["function"] = __func__;
    return createJsonResponse(true, "Stream stopped successfully", responseData);
}