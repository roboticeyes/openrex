project(libopenrex)
set(MAJOR_VERSION 0)
set(MINOR_VERSION 1)
set(PACKAGE libopenrex)

set(VERSION_STRING ${MAJOR_VERSION}.${MINOR_VERSION})
set(VERSION ${VERSION_STRING})

set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# CMake
if("${CMAKE_BUILD_TYPE}" STREQUAL "")
   set(CMAKE_BUILD_TYPE     RelWithDebInfo)
endif("${CMAKE_BUILD_TYPE}" STREQUAL "")
set(CMAKE_COLOR_MAKEFILE ON)
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

if(UNIX)
	add_definitions(-D_POSIX_C_SOURCE=200809L -pedantic -std=c11 -Wall -Wno-format-truncation)
endif()

add_definitions(-DVERSION=\"${VERSION}\")

include(CMakeOptions.txt)

# Testing
enable_testing()

# Debug build
message("-- Build type: ${CMAKE_BUILD_TYPE}")
if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
   add_definitions(-DDEBUG)
endif(${CMAKE_BUILD_TYPE} STREQUAL Debug)

add_subdirectory(src)
add_subdirectory(extensions)

if (TOOLS)
    add_subdirectory(tools)
endif()

if (IMPORTER)
    add_subdirectory(importer)
endif()

if (VIEWER)
    add_subdirectory(viewer)
endif()

if (TESTS)
    add_subdirectory(test)
endif()

set ( LIBOPENREX_INCLUDE_DIR ${includedir} )
set ( LIBOPENREX_INCLUDE_DIRS ${LIBOPENREX_INCLUDE_DIR} )
set ( LIBOPENREX_LIBRARY openrex )
set ( LIBOPENREX_LIBRARIES ${LIBOPENREX_LIBRARY} )
set ( LIBOPENREX_STATIC_LIBRARY openrex.a )
set ( LIBOPENREX_STATIC_LIBRARIES ${LIBOPENREX_STATIC_LIBRARY} )
set ( LIBOPENREX_LIBRARY_DIRS ${libdir} )
set ( LIBOPENREX_ROOT_DIR ${prefix} )
set ( LIBOPENREX_VERSION_STRING ${VERSION_STRING} )
set ( LIBOPENREX_VERSION_MAJOR ${MAJOR_VERSION} )
set ( LIBOPENREX_VERSION_MINOR ${MINOR_VERSION} )

message (STATUS "Summary build options:

    Build static libs: ${STATICLIBS}
    Build tools: ${TOOLS}
    Build viewer: ${VIEWER}
    Build importer: ${IMPORTER}
    ")
