#pragma once
#include "player.hpp"
#include "raylib.h"
#include "upgrades.hpp"
#include "utils.hpp"
#include <cmath>
#include <raymath.h>
#include <string>

class Gun {
public:
  int x = 0;
  int y = 0;
  int w = 15;
  int h = 5;
  float rot = 0.0f;
  float radius = 0;

  float bSpeed = 5.0f;
  float bRicochets = 0.0f;
  float bMaxDist = -1.0f;
  int baseCooldown = 25;
  int cooldown = baseCooldown;

  float dmg = 1.0f;

  float spread = 0.0f;
  int bulletCount = 1;
  float bulletDegrees = 0.0f;

  float knockbackForce = 3.0f;

  int magSizeMax = 8;
  int magBullets = magSizeMax;

  float reloadFrames = 185;
  float reloadFrameCurrent = reloadFrames;

  Vector2 mouse;
  Color c = GREEN;
  Color c2 = RED;

  std::string displayName = "default";

  int cost = 0;

  bool owned = true;

public:
  virtual void draw() {
    DrawRectanglePro({(float)x, (float)y, (float)w, (float)h}, {(float)radius},
                     rot, magBullets != 0 ? c : c2);
  }

  virtual void updateRot(Camera2D &cam, Player &p) {
    Vector2 mousePos = GetScreenMousePos(cam);
    mouse = mousePos;

    Vector2 pCenter = {(float)p.x + p.w / 2.0f, (float)p.y + p.w / 2.0f};

    Vector2 delta = Vector2Subtract(pCenter, mousePos);

    float angle = atan2f(delta.y, delta.x) * RAD2DEG;
    rot = std::fmod(angle + 360.0f, 360.0f);
  }

  virtual void focusOn(int cX, int cY, int r) {
    x = cX;
    y = cY;
    this->radius = r;
  }

  virtual void applyUpgrades(Upgrades &upgrades) {
    this->bSpeed = upgrades.get("bulletspeed")->getCurrent();
    this->bRicochets = upgrades.get("bulletricochet")->getCurrent();
  }
};

class LMG : public Gun {
public:
  LMG() : Gun() {
    this->baseCooldown = 5;
    this->dmg = 0.3;
    this->magSizeMax = 100;
    this->displayName = "LMG";
    this->knockbackForce = 1.0;
    this->reloadFrames = 600;
    this->cost = 50;
    this->owned = false;
    this->spread = 0.1f;
    this->bulletCount = 1;
    this->bulletDegrees = 0;
  }
};

class Shotgun : public Gun {
public:
  Shotgun() : Gun() {
    this->baseCooldown = 40;
    this->dmg = 0.5;
    this->magSizeMax = 4;
    this->displayName = "Shotgun";
    this->knockbackForce = 1.0f;
    this->reloadFrames = 150;
    this->cost = 50;
    this->owned = false;
    this->spread = 0.0f;
    this->bulletCount = 3;
    this->bulletDegrees = 0.4;
    this->bMaxDist = 30;
  }
};
