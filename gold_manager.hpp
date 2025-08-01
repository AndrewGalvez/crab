#pragma once
#include "game_assets.hpp"
#include <cstdlib>
#include <ctime>
#include <raylib.h>
#include <vector>

class Gold {
private:
  int x;
  int y;
  int w = 8;
  int h = 8;

public:
  Gold(int x, int y) : x(x), y(y) {};

  void draw(GameAssets &assets) {
    DrawTexturePro(*assets.fetchTexture("gold"), {0, 0, 8, 8},
                   {(float)x, (float)y, (float)w, (float)h}, {0, 0}, 0.0f,
                   WHITE);
  };

  Rectangle getRect() { return {(float)x, (float)y, (float)w, (float)h}; }
};

class GoldManager {
private:
  std::vector<Gold> golds;
  float spawnChance = 1.0f;

public:
  void draw(GameAssets &assets) {
    for (Gold &g : golds)
      g.draw(assets);
  }

  void trySpawnAtPos(int x, int y, int amount = 1) {
    for (int i = 0; i < amount; i++) {
      srand(time(0));
      double prob = (double)rand() / RAND_MAX;
      if (prob <= spawnChance) {
        golds.push_back(Gold(x, y));
      }
    }
  }

  std::vector<int> isGoldColliding(Rectangle p) {
    std::vector<int> out;
    for (int j = 0; j < golds.size(); j++) {
      if (CheckCollisionRecs(p, golds[j].getRect())) {
        out.push_back(j);
      }
    }
    return out;
  }

  void removeGold(int j) { golds.erase(golds.begin() + j); }
};
