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

#include "server.h"
#include "music.h"
#include <libmumbleclient/Client.hpp>
#include <libmumbleclient/Settings.hpp>
namespace Dolanik {

std::string Server::getHost()
{
  return this->host;
}
std::string Server::getPassword()
{
  return this->password;
}
std::string Server::getPort()
{
  return this->port;
}
std::string Server::getUsername()
{
  return this->username;
}

  
void Server::connect(const std::string& host, const std::string& port, const std::string& username, const std::string& password)
{
  this->host=host;
  this->port=port;
  this->username=username;
  this->password=password;
  this->mumbleClient->Connect(MumbleClient::Settings(host,port,username,password));
}

void Server::disconnect()
{
  this->mumbleClient->Disconnect();
}
boost::shared_ptr< Music > Server::getMusic()
{
  return this->musicAPI;
}

 
Server::Server(MumbleClient::MumbleClient* mc):
mumbleClient(mc),
musicAPI(new Music(mc))
{}
Server::~Server()
{
  this->disconnect();
}
}