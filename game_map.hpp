#pragma once
#include "raylib.h"
#include <array>
#include <fstream>
#include <iostream>
#include <string>

class GameMap {
public:
  std::array<std::array<bool, 20>, 20> tilemap;
  int TILE_SIZE = 32;
  int tile_num = 20;
  int size = TILE_SIZE * 20;

  bool get(int x, int y) { return tilemap[y][x]; }

  void loadFromFile(const char *fileName) {
    std::ifstream f(fileName);
    if (!f.is_open()) {
      std::cout << "Error: Could not open file " << fileName << std::endl;
      return;
    }

    std::string txt;
    int j = 0;
    while (std::getline(f, txt) && j < tile_num) { // Check bounds for rows
      for (int i = 0; i < txt.length() && i < tile_num;
           i++) { // Check bounds for columns
        char c = txt[i];
        tilemap[j][i] = (c == '1');
      }
      j++;
    }
    f.close();
  }

  void setTileSize(int size) {
    this->TILE_SIZE = size;
    this->size = TILE_SIZE * 20;
  }

  void draw() {
    for (int col = 0; col < tile_num; col++) {
      for (int row = 0; row < tile_num; row++) {
        if (tilemap[row][col]) {
          DrawRectangle(col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE,
                        RED);
        }
      }
    }
  }
};
