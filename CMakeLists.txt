project(dolanik)
cmake_minimum_required (VERSION 2.6)

SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules")


set (libmumbleclient_VERSION_MAJOR 0)
set (libmumbleclient_VERSION_MINOR 1)

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

# LibAV
find_package (LibAV REQUIRED)
message (STATUS "LibAV found!")
message (STATUS "Include dirs: " ${LIBAV_INCLUDE_DIRS})
message (STATUS "Libraries: " ${LIBAV_LIBRARIES} ) 
message (STATUS "")

# Mpg123
find_package (MPG123 REQUIRED)
message (STATUS "MPG123 found!")
message (STATUS "Include dirs: " ${MPG123_INCLUDE_DIR} )
message (STATUS "Libraries: " ${MPG123_LIBRARIES})
message (STATUS "")

# ZeroC Ice
find_package (ZeroCIce REQUIRED)
message (STATUS "ZeroCIce found!")
message (STATUS "Libraries: " ${ZeroCIceCore_LIBRARY} ${ZeroCIceUtil_LIBRARY} )
message (STATUS "")

find_package (LibSpotify REQUIRED)
message (STATUS "LibSpotify found!")
message (STATUS "Include dirs: " ${LIBSPOTIFY_INCLUDE_DIR})
message (STATUS "Libraries: " ${LIBSPOTIFY_LIBRARIES})


set(LIBMUMBLE_BUILD_TYPE STATIC)
set(LIBMUMBLECLIENT_LIBRARIES dl pthread)

SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib) 

add_subdirectory(src)

PROTOBUF_GENERATE_CPP(LIBMUMBLECLIENT_PROTO_SOURCES LIBMUMBLECLIENT_PROTO_HEADERS ${LIBMUMBLECLIENT_PROTO})
GET_FILENAME_COMPONENT(LIBMUMBLECLIENT_PROTO_INCLUDE_DIR ${LIBMUMBLECLIENT_PROTO_HEADERS} PATH)

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

add_library(dolan ${dolan_SRC})
target_link_libraries(dolan libmumbleclient mpg123)

add_executable(dolanik ${dolanik_SRC} 
${DOLANIK_ICE_SOURCES})
target_link_libraries(dolanik dolan ${ZeroCIceCore_LIBRARY} ${ZeroCIceUtil_LIBRARY} ${LIBSPOTIFY_LIBRARY_DEBUG} ${LIBAV_LIBRARIES})