 
add_subdirectory(dolanik)
add_subdirectory(libmumbleclient)
add_subdirectory(ice)

list(APPEND dupa_SRC src/misc.cpp)
list(APPEND dolanik_SRC src/main.cpp)

set(dolan_SRC "${dolan_SRC}" PARENT_SCOPE)
set(dolanik_SRC "${dolanik_SRC}" PARENT_SCOPE)

message(STATUS ${dolanik_SRC})

set(LIBMUMBLECLIENT_SOURCES "${LIBMUMBLECLIENT_SOURCES}" PARENT_SCOPE)
set(LIBMUMBLECLIENT_HEADERS "${LIBMUMBLECLIENT_HEADERS}" PARENT_SCOPE)
