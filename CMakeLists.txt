cmake_minimum_required(VERSION 3.1)

project(OptionsParser)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "build")

set(SOURCES optionsparser.cpp)

set(${HEADERS} optionsparser.h)

include_directories(
  .
)

add_library(OptionsParser SHARED ${SOURCES} ${HEADERS})
