project(fsEventController)
cmake_minimum_required(VERSION 3.1.3)
#使能c++11
set(CMAKE_CXX_STANDARD 11)


include_directories(libs/esl/src/include)

file(GLOB eslsrcs libs/esl/src/*.c libs/esl/src/*.cpp)

add_library(esl STATIC ${eslsrcs})
target_link_libraries(esl pthread)

add_executable(fsEventController testclient.c)
target_link_libraries(fsEventController esl pthread)

























