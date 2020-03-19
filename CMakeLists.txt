project(fsEventController)
cmake_minimum_required(VERSION 2.8)
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

set(ToolKit_Root ${CMAKE_CURRENT_SOURCE_DIR}/libs/ZLToolKit/src)
#设置头文件目录
INCLUDE_DIRECTORIES(${ToolKit_Root})

#收集源代码
file(GLOB ToolKit_src_list ${ToolKit_Root}/*/*.cpp ${ToolKit_Root}/*/*.h ${ToolKit_Root}/*/*.c)

#去除win32的适配代码
if (NOT WIN32)
    list(REMOVE_ITEM ToolKit_src_list ${ToolKit_Root}/win32/getopt.c)
else()
     #防止Windows.h包含Winsock.h
    add_definitions(-DWIN32_LEAN_AND_MEAN -DMP4V2_NO_STDINT_DEFS)
endif ()
add_library(zltoolkit STATIC ${ToolKit_src_list})

add_library(esl STATIC ${eslsrcs})
target_link_libraries(esl pthread)

add_library(json STATIC ${jsonsrcs})


add_executable(fsEventController fsEventServer.cpp messageHandler.cpp)
target_link_libraries(fsEventController  zltoolkit esl pthread json curl ssl crypto z)
# target_link_libraries(fsEventController esl pthread libcurl.a ssl crypto z)
























