#pragma once
#include "raylib.h"
#include <array>
#include <fstream>
#include <iostream>
#include <string>

class GameMapRenderTile {
public:
  int w = 32;
  int h = 32;
  int tx = 0;
  int ty = 0;

  void draw(Texture2D *tilemap, int x, int y) {
    if (tx != -1)
      DrawTexturePro(*tilemap, {(float)tx, (float)ty, 8, 8},
                     {(float)x, (float)y, (float)w, (float)h}, {0, 0}, 0.0f,
                     WHITE);
  }

  GameMapRenderTile(int tx, int ty) : tx(tx), ty(ty) {};

  GameMapRenderTile() {};
};

class GameMap {
public:
  std::array<std::array<bool, 20>, 20> tilemap;
  std::array<std::array<GameMapRenderTile, 20>, 20> renderer;
  int TILE_SIZE = 32;
  int tile_num = 20;
  int size = TILE_SIZE * 20;

  GameMap() {};

  bool get(int x, int y) { return tilemap[y][x]; }

  void loadFromFile(const char *fileName) {
    std::ifstream f(fileName);
    if (!f.is_open()) {
      std::cout << "Error: Could not open file " << fileName << std::endl;
      return;
    }

    std::string txt;
    int j = 0;
    while (std::getline(f, txt) && j < tile_num) {
      for (int i = 0; i < txt.length() && i < tile_num; i++) {
        char c = txt[i];
        tilemap[j][i] = (c == '1');
      }
      j++;
    }
    f.close();
  }

  void loadRenderFromFile(const char *fileName) {
    std::ifstream f(fileName);
    if (!f.is_open()) {
      std::cout << "Error: Could not open file " << fileName << std::endl;
      return;
    }

    std::string txt;
    int j = 0;
    while (std::getline(f, txt) && j < tile_num) {
      for (int i = 0; i < txt.length() && i < tile_num; i++) {
        int tx, ty;
        char c = txt[i];
        int num;

        if (c == ' ' || c == '\0') {
          num = -1;
        } else {
          num = c - '0'; // Convert char to int
        }

        if (num == -1) {
          tx = -1;
          ty = -1;
        } else {
          // Fixed tile coordinate calculation
          int nr = num / 3; // Row in tilemap (integer division)
          int nc = num % 3; // Column in tilemap (modulo)

          switch (nc) {
          case 0:
            tx = 0;
            break;
          case 1:
            tx = 8;
            break;
          case 2:
            tx = 16;
            break;
          default:
            tx = 0;
            break;
          }

          switch (nr) {
          case 0:
            ty = 0;
            break;
          case 1:
            ty = 8;
            break;
          case 2:
            ty = 16;
            break;
          default:
            ty = 0;
            break;
          }
        }

        renderer[j][i] = GameMapRenderTile(tx, ty);
      }
      j++;
    }
    f.close();
  }

  void setTileSize(int size) {
    this->TILE_SIZE = size;
    this->size = TILE_SIZE * 20;
  }

  void draw(Texture2D *tilemap) {
    for (int col = 0; col < tile_num; col++) {
      for (int row = 0; row < tile_num; row++) {
        renderer[row][col].draw(tilemap, col * TILE_SIZE, row * TILE_SIZE);
      }
    }
  }
};
