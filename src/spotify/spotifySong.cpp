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

#include "spotifySong.h"
#include "spotify.h"
#include <libspotify/api.h>
#include <iostream>

SpotifySong::SpotifySong(sp_track* track, Spotify* spotify ):
  duration(-1),
  title("Unknown"),
  albumName("Unknown"),
  artistName("Unknown"),
  track(track),
  artist(nullptr),
  album(nullptr),
  spotify(spotify)
{
  boost::lock_guard<boost::mutex> lock(spotify->spotifyApiMutex);
  sp_track_add_ref(this->track);
//   sp_error err = sp_track_error(this->track);
//   bool loaded = sp_track_is_loaded(this->track);
}
bool SpotifySong::loaded()
{
  boost::lock_guard<boost::mutex> lock(this->spotify->spotifyApiMutex);
  return sp_track_is_loaded(this->track);
}

SpotifySong::~SpotifySong()
{
  boost::lock_guard<boost::mutex> lock(this->spotify->spotifyApiMutex);
  sp_track_release(this->track);
  if(this->artist != nullptr)
    sp_artist_release(this->artist);
  if(this->album != nullptr)
    sp_album_release(this->album);
}



int SpotifySong::getDuration()
{
  return this->duration;
}

std::string SpotifySong::getTitle()
{
  return this->title;
}

std::string SpotifySong::getAlbum()
{
  return this->albumName;
}

std::string SpotifySong::getArtist()
{
  return this->artistName;
}
void SpotifySong::play( Dolanik::Music& music )
{
  this->spotify->play(shared_from_this(), music);
}
void SpotifySong::stop()
{
  spotify->stop(shared_from_this());
}

bool SpotifySong::updateMetadata()
{
  if(sp_track_is_loaded(this->track))
  {
    this->duration = sp_track_duration(this->track);
    this->title = sp_track_name(this->track);
    if(this->artist == nullptr)
    {
      this->artist = sp_track_artist(this->track,0);
      sp_artist_add_ref(this->artist);
    }
    if(this->album == nullptr)
    {
      this->album = sp_track_album(this->track);
      sp_album_add_ref(this->album);
    }
  }else
    return false;
  
  if(this->artist != nullptr && sp_artist_is_loaded(this->artist))
    this->artistName = sp_artist_name(this->artist);
  else
    return false;
  
  if(this->album != nullptr && sp_album_is_loaded(this->album))
    this->albumName = sp_album_name(this->album);
  else
    return false;
  
  return true;
}
