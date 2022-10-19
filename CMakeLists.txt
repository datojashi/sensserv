cmake_minimum_required(VERSION 3.5)

project(sensserv LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(sensserv main.cpp)

if(CMAKE_COMPILER_IS_GNUCXX)
   execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
   if (GCC_VERSION VERSION_GREATER 9.0 OR GCC_VERSION VERSION_EQUAL 9.0)
        message(STATUS "C++20 ENABLED.")
        set_property(GLOBAL PROPERTY CXX_STANDARD 17)
        add_definitions(-std=c++2a)
        #add_compile_definitions("__STD_FILESYSTEM__")
    else()
        message(FATAL_ERROR "No appropriate compiler. Project should compiled with C++11 or greater. compiler GCC version > 4.3.")
    endif()
endif(CMAKE_COMPILER_IS_GNUCXX)

install(TARGETS sensserv
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

target_sources(sensserv
    PUBLIC
        sensorthread.cpp
        sensorthread.h
        senstelnet.cpp
        senstelnet.h
        appthread.cpp
        appthread.h
        )

target_include_directories(sensserv PRIVATE ${CMAKE_SOURCE_DIR}, ../AWL)
target_link_libraries(sensserv pthread)
