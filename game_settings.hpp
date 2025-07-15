#pragma once
#include <fstream>

class GameSettings {

public:
  bool musicEnabled = true;

  void saveToFile(const char *fileName) {
    std::ofstream f(fileName);
    f << musicEnabled;
    f.close();
  }

  void loadFromFile(const char *fileName) {
    std::ifstream f(fileName);
    f >> musicEnabled;
    f.close();
  }
};
