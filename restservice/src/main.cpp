#include <fcgiapp.h>
#include <string>
#include <ctime>
#include <sstream>
#include <iostream>
#include <json/json.h>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

std::string get_current_time() {
    std::time_t now = std::time(nullptr);
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buf);
}

std::string get_uri(FCGX_Request& request) {
    char* uri = FCGX_GetParam("REQUEST_URI", request.envp);
    return uri ? std::string(uri) : "/";
}

void handle_hello(FCGX_Request& request) {
    FCGX_FPrintF(request.out,
        "Content-Type: text/html\r\n\r\n"
        "<html><body><h1>Hello from FastCGI!</h1></body></html>\n");
}

void handle_status(FCGX_Request& request) {
    std::string status_json = R"({"status": "ok", "service": "cpp-fastcgi"})";

    FCGX_FPrintF(request.out,
        "Content-Type: application/json\r\n\r\n"
        "%s\n",
        status_json.c_str());
}

void handle_time(FCGX_Request& request) {
    std::string time_str = get_current_time();

    FCGX_FPrintF(request.out,
        "Content-Type: text/plain\r\n\r\n"
        "Current server time is: %s\n",
        time_str.c_str());
}

void handle_resolution(FCGX_Request& request) {
    std::cout << "Resolution request received" << std::endl;
    const char* content_length_str = FCGX_GetParam("CONTENT_LENGTH", request.envp);
    int content_length = content_length_str ? atoi(content_length_str) : 0;

    std::string data;
    if (content_length > 0) {
        data.resize(content_length);
        FCGX_GetStr(&data[0], content_length, request.in);
    }

    Json::CharReaderBuilder builder;
    Json::Value json_data;
    std::string errs;

    std::istringstream s(data);
    if (!Json::parseFromStream(builder, s, &json_data, &errs)) {
        FCGX_FPrintF(request.out, "Status: 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nInvalid JSON data.\n");
        return;
    }

    if (!json_data.isMember("resolution_width") || !json_data["resolution_width"].isInt() ||
        !json_data.isMember("resolution_height") || !json_data["resolution_height"].isInt()) {
        FCGX_FPrintF(request.out, "Status: 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nMissing or invalid 'width'/'height'.\n");
        return;
    }

    int width = json_data["resolution_width"].asInt();
    int height = json_data["resolution_height"].asInt();
    int fps = json_data["fps"].asInt();
    
    // ✅ Save to file
    std::string file_path = "/dev/shm/rest_request.json";
    std::ofstream out(file_path, std::ios::trunc);  // trunc will create if not exists
    if (out.is_open()) {
        Json::StreamWriterBuilder writer;
        out << Json::writeString(writer, json_data);
        out.close();
    } else {
        FCGX_FPrintF(request.out, "Status: 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nFailed to write to file.\n");
        return;
    }

    std::ostringstream oss;
    oss << "Resolution set to: " << width << "x" << height;

    FCGX_FPrintF(request.out,
        "Content-Type: text/plain\r\n\r\n"
        "%s\n",
        oss.str().c_str());
}

void handle_framerate(FCGX_Request& request) {
    const char* content_length_str = FCGX_GetParam("CONTENT_LENGTH", request.envp);
    int content_length = content_length_str ? atoi(content_length_str) : 0;

    std::string data;
    if (content_length > 0) {
        data.resize(content_length);
        FCGX_GetStr(&data[0], content_length, request.in);
    }

    Json::CharReaderBuilder builder;
    Json::Value json_data;
    std::string errs;

    std::istringstream s(data);
    if (!Json::parseFromStream(builder, s, &json_data, &errs)) {
        FCGX_FPrintF(request.out, "Status: 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nInvalid JSON data.\n");
        return;
    }

    if (!json_data.isMember("framerate") || !json_data["framerate"].isInt()) {
        FCGX_FPrintF(request.out, "Status: 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nMissing or invalid 'framerate'.\n");
        return;
    }

    int framerate = json_data["framerate"].asInt();

    // ✅ Save to file
    std::string file_path = "/dev/shm/rest_request.json";
    if (fs::exists(file_path)) {
        std::ofstream out(file_path, std::ios::trunc);  // truncate existing file
        if (out.is_open()) {
            Json::StreamWriterBuilder writer;
            out << Json::writeString(writer, json_data); // write JSON as string
            out.close();
        } else {
            FCGX_FPrintF(request.out, "Status: 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nFailed to write to file.\n");
            return;
        }
    }

    std::ostringstream oss;
    oss << "Framerate set to: " << framerate;

    FCGX_FPrintF(request.out,
        "Content-Type: text/plain\r\n\r\n"
        "%s\n",
        oss.str().c_str());
}
void handle_model(FCGX_Request& request) {
    const char* content_length_str = FCGX_GetParam("CONTENT_LENGTH", request.envp);
    int content_length = content_length_str ? atoi(content_length_str) : 0;

    std::string data;
    if (content_length > 0) {
        data.resize(content_length);
        FCGX_GetStr(&data[0], content_length, request.in);
    }

    Json::CharReaderBuilder builder;
    Json::Value json_data;
    std::string errs;

    std::istringstream s(data);
    if (!Json::parseFromStream(builder, s, &json_data, &errs)) {
        FCGX_FPrintF(request.out, "Status: 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nInvalid JSON data.\n");
        return;
    }

    if (!json_data.isMember("model") || !json_data["model"].isString()) {
        FCGX_FPrintF(request.out, "Status: 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nMissing or invalid 'model'.\n");
        return;
    }

    std::string model = json_data["model"].asString();

    // ✅ Save to file
    std::string file_path = "/dev/shm/rest_request.json";
    if (fs::exists(file_path)) {
        std::ofstream out(file_path, std::ios::trunc);  // truncate existing file
        if (out.is_open()) {
            Json::StreamWriterBuilder writer;
            out << Json::writeString(writer, json_data); // write JSON as string
            out.close();
        } else {
            FCGX_FPrintF(request.out, "Status: 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nFailed to write to file.\n");
            return;
        }
    }

    std::ostringstream oss;
    oss << "Model set to: " << model;

    FCGX_FPrintF(request.out,
        "Content-Type: text/plain\r\n\r\n"
        "%s\n",
        oss.str().c_str());
}
void handle_bitrate(FCGX_Request& request) {
    const char* content_length_str = FCGX_GetParam("CONTENT_LENGTH", request.envp);
    int content_length = content_length_str ? atoi(content_length_str) : 0;

    std::string data;
    if (content_length > 0) {
        data.resize(content_length);
        FCGX_GetStr(&data[0], content_length, request.in);
    }

    Json::CharReaderBuilder builder;
    Json::Value json_data;
    std::string errs;

    std::istringstream s(data);
    if (!Json::parseFromStream(builder, s, &json_data, &errs)) {
        FCGX_FPrintF(request.out, "Status: 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nInvalid JSON data.\n");
        return;
    }

    if (!json_data.isMember("bitrate") || !json_data["bitrate"].isInt()) {
        FCGX_FPrintF(request.out, "Status: 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nMissing or invalid 'bitrate'.\n");
        return;
    }

    int bitrate = json_data["bitrate"].asInt();

    // ✅ Save to file
    std::string file_path = "/dev/shm/rest_request.json";
    if (fs::exists(file_path)) {
        std::ofstream out(file_path, std::ios::trunc);  // truncate existing file
        if (out.is_open()) {
            Json::StreamWriterBuilder writer;
            out << Json::writeString(writer, json_data); // write JSON as string
            out.close();
        } else {
            FCGX_FPrintF(request.out, "Status: 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nFailed to write to file.\n");
            return;
        }
    }

    std::ostringstream oss;
    oss << "Bitrate set to: " << bitrate;

    FCGX_FPrintF(request.out,
        "Content-Type: text/plain\r\n\r\n"
        "%s\n",
        oss.str().c_str());
}

void handle_404(FCGX_Request& request) {
    FCGX_FPrintF(request.out,
        "Status: 404 Not Found\r\n"
        "Content-Type: text/html\r\n\r\n"
        "<html><body><h1>404 Not Found</h1></body></html>\n");
}

int main() {
    FCGX_Request request;
    FCGX_Init();
    FCGX_InitRequest(&request, 0, 0);

    while (FCGX_Accept_r(&request) == 0) {
        std::string uri = get_uri(request);

        if (uri == "/" || uri == "/hello") {
            handle_hello(request);
        } else if (uri == "/status") {
            handle_status(request);
        } else if (uri == "/time") {
            handle_time(request);
        } else if (uri == "/set_resolution") {
            handle_resolution(request);
        } else if (uri == "/set_framerate") {
            handle_framerate(request);
        } else if (uri == "/set_model") {
            handle_model(request);
        } else if (uri == "/set_bitrate") {
            handle_bitrate(request);
        } else {
            handle_404(request);
        }
    }

    return 0;
}
