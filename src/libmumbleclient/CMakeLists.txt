# Project sources
set (LIBMUMBLECLIENT_SOURCES
    src/libmumbleclient/Client.cpp
    src/libmumbleclient/ClientLib.cpp
    src/libmumbleclient/Logging.cpp
    src/libmumbleclient/CryptState.cpp
    src/libmumbleclient/CELTCodec.cpp
    src/libmumbleclient/Library.cpp
    src/libmumbleclient/Audio.cpp
    src/libmumbleclient/Mumble.pb.cc
    PARENT_SCOPE
)

# Project includes
set (LIBMUMBLECLIENT_HEADERS
    src/libmumbleclient/Channel.hpp
    src/libmumbleclient/Client.hpp
    src/libmumbleclient/ClientLib.hpp
    src/libmumbleclient/Logging.hpp
    src/libmumbleclient/Messages.hpp
    src/libmumbleclient/Settings.hpp
    src/libmumbleclient/User.hpp
    src/libmumbleclient/Visibility.hpp
    src/libmumbleclient/CryptState.hpp
    src/libmumbleclient/PacketDataStream.hpp
    src/libmumbleclient/CELTCodec.hpp
    src/libmumbleclient/Library.hpp
    src/libmumbleclient/Audio.hpp
    src/libmumbleclient/Mumble.pb.h
    PARENT_SCOPE
)