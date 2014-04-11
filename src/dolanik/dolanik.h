/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2013  Mateusz "MaxMati" Nowotynski <maxmati4@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#pragma once

#include <boost/scoped_ptr.hpp>

namespace MumbleClient {
class MumbleClientLib;
}
namespace Dolanik {

class Music;
class Server;
class Dolanik
{
public:
    Dolanik();
    void init();
    boost::shared_ptr< Music > getMusic(uint id);
    uint connect(const std::string& host, const std::string& port, const std::string& username, const std::string& password);
    void disconnect(int id);
    virtual void run();
    virtual ~Dolanik();
    std::map<uint,boost::shared_ptr<Server>> getServers();
private:
    void onAuth();
    
    MumbleClient::MumbleClientLib* mumbleClientLib; //TODO: remove singleton
    std::map<uint,boost::shared_ptr<Server>> servers;//TODO mutexs
    uint nextId;
    bool running;
  };
}