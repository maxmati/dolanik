add_subdirectory(dolanik)
add_subdirectory(libmumbleclient)
add_subdirectory(ice)
add_subdirectory(spotify)

list(
  APPEND
  dolanik_SRC
  src/main.cpp
  src/misc.cpp
)

set(dolan_SRC "${dolan_SRC}" PARENT_SCOPE)
set(dolanik_SRC "${dolanik_SRC}" PARENT_SCOPE)
set(dolanik_ICE "${dolanik_ICE}" PARENT_SCOPE)

set(SPOTIFY_SRC "${SPOTIFY_SRC}" PARENT_SCOPE)
message(STATUS "spotify: " ${SPOTIFY_SRC})


set(LIBMUMBLECLIENT_SOURCES "${LIBMUMBLECLIENT_SOURCES}" PARENT_SCOPE)
set(LIBMUMBLECLIENT_HEADERS "${LIBMUMBLECLIENT_HEADERS}" PARENT_SCOPE)
set(LIBMUMBLECLIENT_PROTO "${LIBMUMBLECLIENT_PROTO}" PARENT_SCOPE)