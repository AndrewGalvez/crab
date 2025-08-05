#pragma once
#include "raylib.h"
#include <string>
#include <unordered_map>
class GameAssets {
private:
  std::unordered_map<std::string, Texture2D> textures;
  std::unordered_map<std::string, Shader> shaders;
  std::unordered_map<std::string, Music> musics;
  std::unordered_map<std::string, Sound> sounds;

public:
  void loadAssets() {
    textures["crab"] = LoadTexture("assets/crab.png");
    textures["enemy"] = LoadTexture("assets/bird.png");
    textures["gold"] = LoadTexture("assets/gold_single.png");
    textures["hpotion"] = LoadTexture("assets/potion_health.png");
    textures["u_goldmultiplier"] =
        LoadTexture("assets/upgrade_icons/GoldMultiplier.png");
    textures["u_goldspawns"] =
        LoadTexture("assets/upgrade_icons/GoldSpawns.png");
    textures["u_goldmagnet"] =
        LoadTexture("assets/upgrade_icons/GoldMagnet.png");
    shaders["grain"] = LoadShader(0, "shaders/grain.fs");
    shaders["whitemask"] = LoadShader(0, "shaders/white_mask.fs");
    musics["menu"] = LoadMusicStream("assets/menu.mp3");
    musics["ingame"] = LoadMusicStream("assets/ingame.mp3");
    sounds["select"] = LoadSound("assets/select.wav");
    sounds["hurt"] = LoadSound("assets/explosion.wav");
    sounds["hit"] = LoadSound("assets/hit.wav");
    sounds["shoot"] = LoadSound("assets/click.wav");
    sounds["levelup"] = LoadSound("assets/levelup.wav");
    sounds["pickupGold"] = LoadSound("assets/pickupCoin.wav");
    sounds["hPotion"] = LoadSound("assets/potion_health.wav");
    sounds["death"] = LoadSound("assets/death.wav");
    sounds["goldspawn"] = LoadSound("assets/gold_spawn.wav");
  }

  Texture2D *fetchTexture(const std::string &key) { return &textures[key]; }
  Music *fetchMusic(const std::string &key) { return &musics[key]; }
  Sound *fetchSound(const std::string &key) { return &sounds[key]; }
  Shader *fetchShader(const std::string &key) { return &shaders[key]; }
};
