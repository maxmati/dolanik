/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  <copyright holder> <email>
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

#include "state.h"

#include "server.h"

#include <iostream>
#include <fstream>

using std::string;
using std::map;
using std::make_pair;
using std::ifstream;
using std::ofstream;


State::State( boost::shared_ptr< Dolanik::Dolanik > dolanik, const string& filename )
:dolanik(dolanik)
{
  this->load(filename);
}

State::~State()
{

}

void State::load ( const std::string& filename )
{
  this->filename = filename;
  
  ifstream file(filename);
  if( !this->reader.parse(file,this->root) )
  {
    std::cout  << "Failed to parse last state: /n"
      << reader.getFormatedErrorMessages();
    return;
  }
  
  const Json::Value servers = root["servers"];
  for ( uint i = 0; i< servers.size(); ++i )  // Iterates over the sequence elements.
    this->dolanik->connect( 
      servers[i].get("host","").asString(),
      servers[i].get("port","23768").asString(),
      servers[i].get("username","dolanik").asString(),
      servers[i].get("password","").asString(),
      servers[i].get("certFile","/etc/dolanik/cert.pem").asString()
    );
}
void State::save()
{
  Json::Value servers(Json::arrayValue);
  servers.clear();
  map<uint, boost::shared_ptr<Dolanik::Server>> curServers = this->dolanik->getServers();
  for(auto it = curServers.begin(); it != curServers.end(); ++it)
  {
    Json::Value server(Json::objectValue);
    server["host"] = it->second->getHost();
    server["port"] = it->second->getPort();
    server["username"] = it->second->getUsername();
    server["password"] = it->second->getPassword();
    server["certFile"] = it->second->getCertFile();
    servers[servers.size()] = (server);
  }
  root["servers"] = servers;
  
  std::ofstream file(this->filename);
  
  file<<this->root;

}


