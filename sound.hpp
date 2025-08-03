#pragma once
#include "game_assets.hpp"
#include "raylib.h"

class SoundManager {
public:
  bool enabled = true;
  GameAssets *assets;

  SoundManager(bool enabled, GameAssets *assets)
      : enabled(enabled), assets(assets) {}

  void play(const std::string id) const {
    if (!enabled)
      return;

    PlaySound(*assets->fetchSound(id));
  }
};
