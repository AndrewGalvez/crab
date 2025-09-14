#pragma once
#include "game_assets.hpp"
#include "sound.hpp"
#include "utils.hpp"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <raylib.h>
#include <raymath.h>
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

  void moveTowards(int cx, int cy, float lerpSpeed, float deltaTime) {
    Vector2 current = {(float)x + w / 2.0f, (float)y + h / 2.0f};
    Vector2 target = {(float)cx, (float)cy};

    float lerpFactor = lerpSpeed * deltaTime;
    if (lerpFactor > 1.0f)
      lerpFactor = 1.0f;

    Vector2 newPos = Vector2Lerp(current, target, lerpFactor);

    x = newPos.x - w / 2.0f;
    y = newPos.y - h / 2.0f;
  }
};

class GoldManager {
private:
  float spawnChance = 1.0f;

public:
  std::vector<Gold> golds;
  void draw(GameAssets &assets) {
    for (Gold &g : golds)
      g.draw(assets);
  }

  void trySpawnAtPos(int x, int y, SoundManager &s, int amount = 1) {
    for (int i = 0; i < amount; i++) {
      srand(time(0));
      double prob = (double)rand() / RAND_MAX;
      if (prob <= spawnChance) {
        int ox = GetRandomValue(-10, 10);
        int oy = GetRandomValue(-10, 10);
        golds.push_back(Gold(x + ox, y + oy));
        s.play("goldspawn");
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

  void setSpawnChance(float a) { spawnChance = a; }

  void updateGoldMagnet(int pcx, int pcy, float gmr) {
    for (Gold &g : golds) {
      Rectangle gr = g.getRect();
      if (Vector2Distance({(float)pcx, (float)pcy},
                          {gr.x + gr.width / 2, gr.y + gr.height / 2}) <= gmr) {
        g.moveTowards(pcx, pcy, 5.0f, GetFrameTime());
      }
    }
  }
};
