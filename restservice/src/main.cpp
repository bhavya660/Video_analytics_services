#include "RestService.hpp"
#include "Logger.hpp"
#include <fcgiapp.h>
#include <json/json.h>
#include <string>
#include <sstream>
#include <unordered_map>

// Utility to parse query string into a JSON object (e.g., "stream_id=stream1" -> {"stream_id": "stream1"})
Json::Value parseQueryString(const std::string& query) {
    Json::Value result;
    if (query.empty()) {
        return result;
    }

    std::unordered_map<std::string, std::string> params;
    std::stringstream ss(query);
    std::string pair;

    while (std::getline(ss, pair, '&')) {
        size_t pos = pair.find('=');
        if (pos != std::string::npos) {
            std::string key = pair.substr(0, pos);
            std::string value = pair.substr(pos + 1);
            params[key] = value;
        }
    }

    for (const auto& [key, value] : params) {
        result[key] = value;
    }
    return result;
}

int main() {
    LOG_INFO("Starting REST service...");

    FCGX_Init();
    FCGX_Request request;
    FCGX_InitRequest(&request, 0, 0);

    RestService restService;

    while (FCGX_Accept_r(&request) >= 0) {
        std::string method = FCGX_GetParam("REQUEST_METHOD", request.envp) ? FCGX_GetParam("REQUEST_METHOD", request.envp) : "";
        std::string path = FCGX_GetParam("REQUEST_URI", request.envp) ? FCGX_GetParam("REQUEST_URI", request.envp) : "";
        std::string contentLengthStr = FCGX_GetParam("CONTENT_LENGTH", request.envp) ? FCGX_GetParam("CONTENT_LENGTH", request.envp) : "0";

        // Remove query string from path (e.g., "/get_stream_status?stream_id=stream1" -> "/get_stream_status")
        size_t queryPos = path.find('?');
        if (queryPos != std::string::npos) {
            path = path.substr(0, queryPos);
        }

        Json::Value requestBody;
        if (method == "GET") {
            // Parse query string for GET requests
            std::string queryString = FCGX_GetParam("QUERY_STRING", request.envp) ? FCGX_GetParam("QUERY_STRING", request.envp) : "";
            requestBody = parseQueryString(queryString);
        }
        else if (method == "POST") {
            // Parse body for POST requests
            int contentLength = std::stoi(contentLengthStr);
            std::string requestBodyStr;
            if (contentLength > 0) {
                std::vector<char> buffer(contentLength);
                FCGX_GetStr(buffer.data(), contentLength, request.in);
                requestBodyStr.assign(buffer.data(), contentLength);
            }

            Json::CharReaderBuilder readerBuilder;
            std::unique_ptr<Json::CharReader> reader(readerBuilder.newCharReader());
            std::string errors;
            if (!requestBodyStr.empty() && !reader->parse(requestBodyStr.data(), requestBodyStr.data() + requestBodyStr.size(), &requestBody, &errors)) {
                LOG_ERROR("JSON parsing error: ", errors);
                FCGX_PutS("Content-Type: application/json\r\n\r\n", request.out);
                FCGX_PutS(restService.createErrorResponse("Invalid JSON: " + errors).c_str(), request.out);
                FCGX_Finish_r(&request);
                continue;
            }
        }

        std::string response = restService.processRequest(method, path, requestBody);

        FCGX_PutS("Content-Type: application/json\r\n\r\n", request.out);
        FCGX_PutS(response.c_str(), request.out);

        FCGX_Finish_r(&request);
    }

    LOG_INFO("REST service shutting down...");
    return 0;
}