cmake_minimum_required(VERSION 3.14)
project(MyMicroservice)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include header files
include_directories(inc)

# Add executable
add_executable(VAService
    src/main.cpp
    src/VideoCapture.cpp
    src/InferenceEngine.cpp
    src/Logger.cpp
)
