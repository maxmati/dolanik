/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2013  <copyright holder> <email>

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



#include "../libmumbleclient/Client.hpp"
#include "../libmumbleclient/ClientLib.hpp"
#include "../libmumbleclient/Settings.hpp"
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "music.h"

#include "dolan.h"


namespace Dolanik {

Dolan::Dolan()
    :mumbleClientLib(MumbleClient::MumbleClientLib::instance()),
     mumbleClient(mumbleClientLib->NewClient()),
     music( new Music (mumbleClient.get()))
{
    mumbleClientLib->SetLogLevel(3);
    mumbleClient->Connect(MumbleClient::Settings("195.64.130.69", "64738", "Dolanik4", ""));
    //mumbleClient->Connect(MumbleClient::Settings("127.0.0.1", "64738", "Dolanik4", ""));
    mumbleClient->SetAuthCallback(boost::bind(&Dolan::onAuth, this));


}

void Dolan::init()
{
    //Threads::Pool::getInstance()->addTask(shared_from_this());
    boost::thread(boost::bind(&Dolan::run,this));

}


void Dolan::onAuth()
{
    //Threads::Pool::getInstance()->addTask(this->music);
}

void Dolan::run()
{
    mumbleClientLib->Run();
}

boost::shared_ptr< Music > Dolan::Dolan::getMusic()
{
  return this->music;
}


Dolan::~Dolan()
{
    mumbleClientLib->Shutdown();
}
}
