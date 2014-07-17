project(dolanik)
cmake_minimum_required (VERSION 2.6)

SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules")


set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)

option(USE_SPOTIFY 
  "Use libspotify for playing music from spotify." OFF)

configure_file (
  "${PROJECT_SOURCE_DIR}/src/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
)
  
set (CMAKE_BUILD_TYPE Debug Release)

SET(CMAKE_CXX_FLAGS "-g -std=c++0x -Wall")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
find_package( Boost 1.42 COMPONENTS system date_time regex chrono serialization thread random filesystem REQUIRED )
message (STATUS "Boost found!")
message (STATUS "Include dirs: " ${Boost_INCLUDE_DIRS})
message (STATUS "")

find_package (OpenSSL REQUIRED)
message (STATUS "OpenSSL found!")
message (STATUS "Include dirs: " ${OPENSSL_INCLUDE_DIR})
message (STATUS "Libraries: ${OPENSSL_LIBRARIES}")
message (STATUS "")

# Protobuf
find_package (Protobuf REQUIRED)
message (STATUS "Protobuf found!")
message (STATUS "Include dirs: " ${PROTOBUF_INCLUDE_DIRS})
message (STATUS "Libraries: " ${PROTOBUF_LIBRARIES})
message (STATUS "Protoc executable: " ${PROTOBUF_PROTOC_EXECUTABLE})
message (STATUS "")

# Crypt
find_package (Crypt REQUIRED)
message (STATUS "Crypt found!")
message (STATUS "Include dirs: " ${CRYPT_INCLUDE_DIR})
message (STATUS "Libraries: " ${CRYPT_LIBRARIES})
message (STATUS "")

# Celt
pkg_check_modules(CELT REQUIRED celt>=0.11.0)
message (STATUS "Celt found!")
message (STATUS "Version: " ${CELT_VERSION})
message (STATUS "Include dirs: " ${CELT_INCLUDEDIR})
message (STATUS "")

# FFmpeg
FIND_PACKAGE(FFmpeg REQUIRED)
message (STATUS "FFmpeg found!")
message (STATUS "FFmpeg Include dirs: " ${FFMPEG_INCLUDE_DIRS})
message (STATUS "FFmpeg Libraries: " ${FFMPEG_LIBRARIES} )  
message (STATUS "")

# LibAVFilter
FIND_PACKAGE(LibAVFilter REQUIRED)
message (STATUS "AVFilter found!")
message (STATUS "AVFilter Include dirs: " ${AVFILTER_INCLUDE_DIRS})
message (STATUS "AVFilter Libraries: " ${AVFILTER_LIBRARY} ) 
message (STATUS "")

# ZeroC Ice
find_package (ZeroCIce REQUIRED)
message (STATUS "ZeroCIce found!")
message (STATUS "Libraries: " ${ZeroCIceCore_LIBRARY} ${ZeroCIceUtil_LIBRARY} )
message (STATUS "")

# ZeroC Ice
find_package (JsonCpp REQUIRED)
message (STATUS "Jsoncpp found!")
message (STATUS "Include dirs: " ${JSONCPP_INCLUDE_DIRS})
message (STATUS "Libraries: " ${JSONCPP_LIBRARIES})
message (STATUS "")

if(USE_SPOTIFY)
  message(STATUS "Compiling with spotify support")
  
  find_package (LibSpotify REQUIRED)
  message (STATUS "LibSpotify found!")
  message (STATUS "Include dirs: " ${LIBSPOTIFY_INCLUDE_DIR})
  message (STATUS "Libraries: " ${LIBSPOTIFY_LIBRARIES})
  
  set(EXTRA_LIBRARIES ${EXTRA_LIBRARIES} ${LIBSPOTIFY_LIBRARY_DEBUG})
  set(EXTRA_INCLUDE_DIR ${EXTRA_INCLUDE_DIR} ${LIBSPOTIFY_INCLUDE_DIR})
endif(USE_SPOTIFY)


set(LIBMUMBLE_BUILD_TYPE STATIC)
set(LIBMUMBLECLIENT_LIBRARIES dl pthread)

SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib) 

add_subdirectory(src)

if(USE_SPOTIFY)
  set(EXTRA_SRC ${EXTRA_SRC} ${SPOTIFY_SRC})
endif(USE_SPOTIFY)


PROTOBUF_GENERATE_CPP(LIBMUMBLECLIENT_PROTO_SOURCES LIBMUMBLECLIENT_PROTO_HEADERS ${LIBMUMBLECLIENT_PROTO})
GET_FILENAME_COMPONENT(LIBMUMBLECLIENT_PROTO_INCLUDE_DIR ${LIBMUMBLECLIENT_PROTO_HEADERS} PATH)

execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/src/ice/slice")


SLICE_GENERATE_CPP( DOLANIK_ICE_SOURCES DOLANIK_ICE_HEADERS  ${dolanik_ICE})
GET_FILENAME_COMPONENT(DOLANIK_ICE_INCLUDE_DIR ${DOLANIK_ICE_HEADERS} PATH)

set(DOLANIK_ICE_INCLUDE_DIR "${PROJECT_BINARY_DIR}/${DOLANIK_ICE_INCLUDE_DIR}")

INCLUDE_DIRECTORIES(
  src/
  ${Boost_INCLUDE_DIR}
  ${OPENSSL_INCLUDE_DIR}
  ${CELT_INCLUDE_DIR} 
  ${PROTOBUF_INCLUDE_DIR}
  ${CRYPT_INCLUDE_DIR}
  ${LIBMUMBLECLIENT_PROTO_INCLUDE_DIR}
  ${DOLANIK_ICE_INCLUDE_DIR}
  ${FFMPEG_INCLUDE_DIRS}
  ${AVFILTER_INCLUDE_DIRS}
  ${JSONCPP_INCLUDE_DIRS}
  ${EXTRA_INCLUDE_DIR}
)

add_library(
  libmumbleclient
  ${LIBMUMBLE_BUILD_TYPE}
  ${LIBMUMBLECLIENT_SOURCES}
  ${LIBMUMBLECLIENT_HEADERS}
  ${LIBMUMBLECLIENT_PROTO_SOURCES}
  ${LIBMUMBLECLIENT_PROTO_HEADERS}
)
target_link_libraries(
  libmumbleclient
  ${LIBMUMBLECLIENT_LIBRARIES}
  ${Boost_LIBRARIES}
  ${OPENSSL_LIBRARIES}
  ${PROTOBUF_LIBRARY}
  ${CRYPT_LIBRARIES}
)

add_library(
  dolan 
  ${dolan_SRC}
)
target_link_libraries(
  dolan
  libmumbleclient
)

add_executable(
  dolanik
  ${dolanik_SRC} 
  ${DOLANIK_ICE_SOURCES}
  ${EXTRA_SRC}
)
target_link_libraries(
  dolanik
  dolan
  ${ZeroCIceCore_LIBRARY} 
  ${ZeroCIceUtil_LIBRARY}
  ${EXTRA_LIBRARIES} 
  ${FFMPEG_LIBRARIES}
  ${AVFILTER_LIBRARIES}
  ${JSONCPP_LIBRARIES}
  opus
)

INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/dolanik
  DESTINATION bin)
INSTALL(FILES dist/etc/dolanik/config
  DESTINATION etc/dolanik)