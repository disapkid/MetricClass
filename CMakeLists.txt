cmake_minimum_required(VERSION 3.10.2)
project(Metric)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_library(Metric STATIC 
    src/metrics.cpp
    include/metrics.hpp)
target_include_directories(Metric PUBLIC include)

add_executable(Main src/main.cpp)
target_link_libraries(Main PRIVATE Metric)

target_compile_definitions(Main PRIVATE
    LOG_DIR=\"${CMAKE_SOURCE_DIR}/logs\"
)
