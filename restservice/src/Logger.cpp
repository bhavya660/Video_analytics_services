#include "Logger.hpp"

// Singleton implementation
Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}