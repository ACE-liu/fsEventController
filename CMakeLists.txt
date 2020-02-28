project(fsEventController)
cmake_minimum_required(VERSION 3.1.3)
#使能c++11
#set(CMAKE_CXX_STANDARD 11)
add_compile_options(-std=c++11)


include_directories(libs/esl/src/include)
include_directories(libs/)
include_directories(libs/curl/include)
include_directories(include/)

link_directories(libs/curl/lib)

file(GLOB eslsrcs libs/esl/src/*.c libs/esl/src/*.cpp)

file(GLOB jsonsrcs libs/jsoncpp/*.cpp libs/jsoncpp/*.h)

add_library(esl STATIC ${eslsrcs})
target_link_libraries(esl pthread)

add_library(json STATIC ${jsonsrcs})


add_executable(fsEventController testclient.cpp messageHandler.cpp)
target_link_libraries(fsEventController esl pthread json curl ssl crypto z)
# target_link_libraries(fsEventController esl pthread libcurl.a ssl crypto z)
























