#pragma once

#include <string>
#include <map>

namespace Dolanik {
class Config{
private:
  typedef std::map<std::string,std::string> mapT;
  
public:
  static Config* getInstance();
  void parseConfig(std::string path);
  std::string getAsString(std::string key, std::string fallback = "");
protected:
private:
  Config();
  ~Config();
  void parseLine ( std::string line );

private:
  static Config* instance;
  
  mapT values;
  
};
}