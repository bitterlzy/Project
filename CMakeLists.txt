#定义最低CMake版本要求
cmake_minimum_required(VERSION 2.8)
#定义项目名称
project(Programs_Mine)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#设置C++标准
if(NOT ARCH STREQUAL "arm")
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED True)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Wall -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O2")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a") 

#W32配置
if(WIN32)
    execute_process(COMMAND updateSVN.bat )
    set(HEADERS svnver.h)
else()
    execute_process(COMMAND svnversion -n OUTPUT_VARIABLE SVN_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    add_definitions(-DSVN_VERSION="${SVN_VERSION}")
endif()


# 根据编译器路径执行不同的配置
if(ARCH STREQUAL "aarch64_3568")
    # 获取编译器的绝对路径
    get_filename_component(CXX_COMPILER_PATH ${CMAKE_CXX_COMPILER} ABSOLUTE)
    set(CMAKE_SYSROOT "/opt/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/sysroot")
endif()
add_definitions(-DARCH="${ARCH}")

# 添加包含路径
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Programs
    ${CMAKE_SOURCE_DIR}/Managers
)
file(GLOB_RECURSE ALL_FILES RELATIVE 
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Programs/*.h
    ${CMAKE_SOURCE_DIR}/Programs/*.hpp
    ${CMAKE_SOURCE_DIR}/Programs/*.c
    ${CMAKE_SOURCE_DIR}/Programs/*.cpp
    ${CMAKE_SOURCE_DIR}/Managers/*.h
    ${CMAKE_SOURCE_DIR}/Managers/*.hpp
    ${CMAKE_SOURCE_DIR}/Managers/*.c
    ${CMAKE_SOURCE_DIR}/Managers/*.cpp
    ${CMAKE_SOURCE_DIR}/Programs/cproject.h
    ${CMAKE_SOURCE_DIR}/Programs/cproject.cpp

    ${CMAKE_SOURCE_DIR}/main.cpp
)

# set( ALL_FILES
#     ${CMAKE_SOURCE_DIR}/main.cpp
#     ${CMAKE_SOURCE_DIR}/Programs/globalfunc.h
#     ${CMAKE_SOURCE_DIR}/Programs/globalfunc.cpp
#     ${CMAKE_SOURCE_DIR}/Programs/chttpserver.h
#     ${CMAKE_SOURCE_DIR}/Programs/chttpserver.cpp

# )


message(STATUS "CMAKE_SORCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "ALL_FILES: ${ALL_FILES}")

add_executable(${PROJECT_NAME} ${ALL_FILES})

if(ARCH STREQUAL "aarch64_3568")
    target_link_libraries(${PROJECT_NAME} PRIVATE rt dl pthread uuid)

elseif(ARCH STREQUAL "atmel")
    add_definitions(-DDB_ROOT_PATH="/home/data/sysdata" -DATMEL)
    target_link_libraries(${PROJECT_NAME} PRIVATE rt dl pthread)

elseif(ARCH STREQUAL "armA9_IEI")
    target_link_libraries(${PROJECT_NAME} PRIVATE rt dl pthread)

elseif(ARCH STREQUAL "arm")
    add_definitions(-DDB_ROOT_PATH="/home/data/sysdata")
    target_link_libraries(${PROJECT_NAME} PRIVATE rt dl pthread)

elseif(ARCH STREQUAL "x86")
    target_link_libraries(${PROJECT_NAME} PRIVATE rt dl pthread)

elseif(ARCH STREQUAL "x86_64")
    target_link_libraries(${PROJECT_NAME} PRIVATE rt dl pthread)
    
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE rt dl pthread uuid mysqlclient jsoncpp)
endif()

