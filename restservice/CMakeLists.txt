# cmake_minimum_required(VERSION 3.10)
# project(FastCGIApp)

# set(CMAKE_CXX_STANDARD 11)

# # Include header files
# # include_directories(inc)

# # Add the executable and all source files
# add_executable(rest_service
#     src/main.cpp
#     # src/RestService.cpp
# )

# # Find and link the FastCGI library
# find_library(FCGI_LIB fcgi REQUIRED)
# target_link_libraries(rest_service ${FCGI_LIB})


cmake_minimum_required(VERSION 3.10)
project(FastCGIApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_STANDARD 11)

include_directories(inc)

# Add the executable and all source files
add_executable(rest_service
    src/main.cpp
    src/RestService.cpp
    src/Logger.cpp
)

# Find and link the FastCGI library
find_library(FCGI_LIB fcgi REQUIRED)
target_link_libraries(rest_service ${FCGI_LIB})

# Link jsoncpp
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)

if (JSONCPP_FOUND)
    include_directories(${JSONCPP_INCLUDE_DIRS})
    target_link_libraries(rest_service ${JSONCPP_LIBRARIES})
else()
    message(FATAL_ERROR "jsoncpp not found")
endif()
