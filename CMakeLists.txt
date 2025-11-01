cmake_minimum_required(VERSION 4.0)
project(Pitaya)

set(CMAKE_CXX_STANDARD 23)

set(BOOST_ROOT "/opt/local/libexec/boost/1.88/")


find_package(Boost 1.88)

if (NOT Boost_FOUND)
    message(FATAL_ERROR, "Could not find boost!")
endif ()
set (Boost_USE_STATIC_LIBS OFF)
#message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
#message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
#message(STATUS "Boost_VERSION: ${Boost_VERSION}")

include_directories(${Boost_INCLUDE_DIRS})

add_executable(Pitaya tokens.cpp main.cpp)
add_subdirectory(Boost_tests)