#include "config.h"

#include <iostream>
#include <fstream>
#include <vector>

#include <boost/algorithm/string.hpp>


namespace Dolanik {

Config* Config::instance = nullptr;
using std::ifstream;
using std::string;
using std::vector;
using boost::algorithm::is_any_of;
  
Config* Config::getInstance()
{
  if(!instance)
    instance = new Config();
  
  return instance;
}

Config::Config(){}
Config::~Config(){}

void Config::parseConfig ( std::string path )
{
  ifstream file(path);
  if(!file.good())
    return; //TODO: Exception
  
  std::string line;
  
  while(getline(file, line))
    parseLine(line);
  
}

string Config::getAsString ( std::string key, std::string fallback )
{
  if(values.count(key) == 0)
    return fallback;
  
  return values[key];
}


void Config::parseLine ( string line )
{
  boost::algorithm::trim(line);
  
  if(line.size() == 0) return;
  if(line[0] == '#') return;
  
  vector<string> strings;
  boost::algorithm::split(strings, line, is_any_of("="));
  
  if(strings.size() != 2) return;
  
  values[strings[0]] = strings[1];
}




}