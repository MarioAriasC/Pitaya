# This file handles building each task as sub-project.
#
# Each task becomes an independent project in which you can experiment.
#
# Tasks are added as separate sub-projects so as compilation errors in
# one task don't affect another task.


project(Pitaya)

set(HEADER_FILES tokens.h lexer.h parser.h)

set(SOURCE_FILES tokens.cpp lexer.cpp parser.cpp)


# Add tasks subprojects
#include(cmake/utils.cmake)
#add_subprojects(${CMAKE_SOURCE_DIR})

add_library(Pitaya_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})