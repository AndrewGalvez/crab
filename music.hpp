#pragma once
#include "game_assets.hpp"
#include "raylib.h"

class MusicPlayer {
private:
  const char *name;

public:
  MusicPlayer(const char *name) : name(name) {};

  Music &fetch(GameAssets *assets) { return *assets->fetchMusic(name); }

  void stopMusic(GameAssets *assets) {
    StopMusicStream(*assets->fetchMusic(name));
  }

  void startMusic(GameAssets *assets) {
    PlayMusicStream(*assets->fetchMusic(name));
  }

  void resumeMusic(GameAssets *assets) {
    ResumeMusicStream(*assets->fetchMusic(name));
  }

  void updateMusic(GameAssets *assets) {
    UpdateMusicStream(*assets->fetchMusic(name));
  }
};

class MusicManager {
public:
  MusicPlayer currentTrack = MusicPlayer("menu");

  void swapTrack(const char *next) { currentTrack = MusicPlayer(next); }
};
