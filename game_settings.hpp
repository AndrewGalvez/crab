#pragma once
#include <fstream>
#include <iostream>

class GameSettings {
public:
  bool musicEnabled = true;
  bool soundEnabled = true;

  void saveToFile(const char *fileName) {
    std::ofstream f(fileName);
    f << musicEnabled << ' ' << soundEnabled;
    f.close();
  }

  void loadFromFile(const char *fileName) {
    std::ifstream f(fileName);
    f >> musicEnabled >> soundEnabled;
    f.close();
  }
};
