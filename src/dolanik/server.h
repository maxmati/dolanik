/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2013  <copyright holder> <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

namespace MumbleClient {
class MumbleClient;
}

namespace Dolanik
{

class Music;
class Server
{
public:
    Server(MumbleClient::MumbleClient* mc);
    ~Server();
    
    void connect(const std::string& host, const std::string& port, const std::string& username, const std::string& password, const std::string& certFile);
    void disconnect();
    
    boost::shared_ptr<Music> getMusic();
    std::string getHost();
    std::string getPort();
    std::string getUsername();
    std::string getPassword();
private:
    boost::scoped_ptr<MumbleClient::MumbleClient> mumbleClient;
    boost::shared_ptr<Music> musicAPI;
    std::string host;
    std::string port;
    std::string username;
    std::string password;

};
}
