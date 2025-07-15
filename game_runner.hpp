#pragma once
#include "game_assets.hpp"
#include "player.hpp"
#include "raylib.h"

class GameRunner {
private:
  Player p = Player(10, 10, 32, 32);

public:
  void draw(GameAssets *assets, int frame) {
    p.draw(assets->fetchTexture("crab"), frame);
    EndShaderMode();
  };

  void update() { p.update(GetFrameTime()); }
};
