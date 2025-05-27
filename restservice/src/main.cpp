#include "RestService.hpp"
#include "Logger.hpp"
#include <fcgiapp.h>
#include <json/json.h>
#include <string>
#include <sstream>

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

        int contentLength = std::stoi(contentLengthStr);
        std::string requestBody;
        if (contentLength > 0) {
            std::vector<char> buffer(contentLength);
            FCGX_GetStr(buffer.data(), contentLength, request.in);
            requestBody.assign(buffer.data(), contentLength);
        }

        Json::Value jsonBody;
        Json::CharReaderBuilder readerBuilder;
        std::unique_ptr<Json::CharReader> reader(readerBuilder.newCharReader());
        std::string errors;
        if (!requestBody.empty() && !reader->parse(requestBody.data(), requestBody.data() + requestBody.size(), &jsonBody, &errors)) {
            LOG_ERROR("JSON parsing error: ", errors);
            FCGX_PutS("Content-Type: application/json\r\n\r\n", request.out);
            FCGX_PutS(restService.createErrorResponse("Invalid JSON: " + errors).c_str(), request.out);
            FCGX_Finish_r(&request);
            continue;
        }

        std::string response = restService.processRequest(method, path, jsonBody);

        FCGX_PutS("Content-Type: application/json\r\n\r\n", request.out);
        FCGX_PutS(response.c_str(), request.out);

        FCGX_Finish_r(&request);
    }

    LOG_INFO("REST service shutting down...");
    return 0;
}