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


#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include <libmumbleclient/ClientLib.hpp>

#include "dolan.h"
#include "server.h"


namespace Dolanik {
std::map< uint, boost::shared_ptr< Server > > Dolan::getServers()
{
  return this->servers;
}


Dolan::Dolan():
mumbleClientLib(MumbleClient::MumbleClientLib::instance()),
nextId(0)
{
    mumbleClientLib->SetLogLevel(3);
}
void Dolan::disconnect(int id)
{
  servers.at(id)->disconnect();
  servers.erase(id);
}

uint Dolan::connect(const std::string& host, const std::string& port, const std::string& username, const std::string& password)
{
  uint id = nextId++;
  boost::shared_ptr<Server> server(new Server(mumbleClientLib->NewClient()));
  servers.insert(std::pair<uint, boost::shared_ptr<Server> > (id, server));
  servers.at(id)->connect(host,port,username,password);
  return id;
}


void Dolan::init()
{
    boost::thread(boost::bind(&Dolan::run,this));
}


void Dolan::onAuth()
{
}

void Dolan::run()
{
  this->running = true;
  while(this->running)
    mumbleClientLib->Run();
}

boost::shared_ptr< Music > Dolan::Dolan::getMusic(uint id)
{
  return this->servers.at(id)->getMusic();
}


Dolan::~Dolan()
{
  this->running = false;
  mumbleClientLib->Shutdown();
}
}
