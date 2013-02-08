/*
    Dupa - home servers farm controler
    Copyright (C) 2012  Mateusz "Maxmati" Nowotynski <maxmati4@gmail.com>

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

#include <string>
#include <sstream>

#include "misc.h"

template <typename T>
std::string Anal::toStr(const T input)
{
  std::string str;
  std::stringstream stream;

  stream << input;
  stream >> str;

  return str;
}

template std::string Anal::toStr<bool>(const bool input);
template std::string Anal::toStr<int>(const int input);
template std::string Anal::toStr<float>(const float input);
template std::string Anal::toStr<uint32_t>(const uint32_t input);

template <typename T>
T Anal::fromStr(const std::string input)
{
//  std::string str;
  T out;
  std::stringstream stream;

  stream << input;
  stream >> out;

  return out;
}

template long int Anal::fromStr<long int>(const std::string input);
template uint Anal::fromStr<uint>(const std::string input);