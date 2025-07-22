#pragma once
#include "raylib.h"
#include <unordered_map>
class GameAssets {
private:
  std::unordered_map<const char *, Texture2D> textures;
  std::unordered_map<const char *, Shader> shaders;
  std::unordered_map<const char *, Music> musics;
  std::unordered_map<const char *, Sound> sounds;

public:
  void loadAssets() {
    textures["crab"] = LoadTexture("assets/crab.png");
    textures["enemy"] = LoadTexture("assets/bird.png");
    shaders["grain"] = LoadShader(0, "shaders/grain.fs");
    shaders["whitemask"] = LoadShader(0, "shaders/white_mask.fs");
    musics["menu"] = LoadMusicStream("assets/menu.mp3");
    musics["ingame"] = LoadMusicStream("assets/ingame.mp3");
    sounds["select"] = LoadSound("assets/select.wav");
    sounds["hurt"] = LoadSound("assets/explosion.wav");
    sounds["hit"] = LoadSound("assets/hit.wav");
    sounds["shoot"] = LoadSound("assets/click.wav");
  }

  Texture2D *fetchTexture(const char *key) { return &textures[key]; }
  Music *fetchMusic(const char *key) { return &musics[key]; }
  Sound *fetchSound(const char *key) { return &sounds[key]; }
  Shader *fetchShader(const char *key) { return &shaders[key]; }
};
