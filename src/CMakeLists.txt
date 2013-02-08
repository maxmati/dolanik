 
add_subdirectory(dolanik)
add_subdirectory(libmumbleclient)

list(APPEND dupa_SRC src/misc.cpp)

set(dolan_SRC "${dolan_SRC}" PARENT_SCOPE)
set(LIBMUMBLECLIENT_SOURCES "${LIBMUMBLECLIENT_SOURCES}" PARENT_SCOPE)
set(LIBMUMBLECLIENT_HEADERS "${LIBMUMBLECLIENT_HEADERS}" PARENT_SCOPE)