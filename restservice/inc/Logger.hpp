#pragma once

#include <iostream>
#include <sstream>
#include <mutex>
#include <string>
#include <chrono>
#include <iomanip>

class Logger {
public:
    enum class Level { DEBUG, INFO, WARNING, ERROR };

    static Logger& getInstance();

    template <typename... Args>
    void log(Level level, const char* func, int line, Args&&... args) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::ostringstream oss;
        (oss << ... << std::forward<Args>(args));

        const auto now = std::chrono::system_clock::now();
        const std::time_t time = std::chrono::system_clock::to_time_t(now);
        std::tm buf{};
#if defined(_MSC_VER)
        localtime_s(&buf, &time);
#else
        localtime_r(&time, &buf);
#endif

        std::cout << "[" << std::put_time(&buf, "%Y-%m-%d %H:%M:%S") << "]"
                  << " [" << levelToString(level) << "]"
                  << " [" << func << ":" << line << "] "
                  << oss.str() << std::endl;
    }

private:
    Logger() = default;
    std::mutex mutex_;

    std::string levelToString(Level level) {
        switch (level) {
            case Level::DEBUG:   return "DEBUG";
            case Level::INFO:    return "INFO";
            case Level::WARNING: return "WARNING";
            case Level::ERROR:   return "ERROR";
        }
        return "UNKNOWN";
    }
};

// Logging macros
#define LOG_DEBUG(...)   Logger::getInstance().log(Logger::Level::DEBUG, __func__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)    Logger::getInstance().log(Logger::Level::INFO, __func__, __LINE__, __VA_ARGS__)
#define LOG_WARNING(...) Logger::getInstance().log(Logger::Level::WARNING, __func__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...)   Logger::getInstance().log(Logger::Level::ERROR, __func__, __LINE__, __VA_ARGS__)
