#pragma once
#include "player.hpp"
#include "raylib.h"
#include <cmath>
#include <raymath.h>

class Gun {
public:
  int x = 0;
  int y = 0;
  int w = 15;
  int h = 5;
  float rot = 0.0f;
  float radius = 0;

  float bSpeed = 5.0f;
  int baseCooldown = 25;
  int cooldown = baseCooldown;

  Vector2 mouse;
  Color c = GREEN;

  void draw() {
    // Draw the gun rectangle with proper pivot point (center of the gun)
    DrawRectanglePro({(float)x, (float)y, (float)w, (float)h}, {(float)radius},
                     rot, c);
  }

  void updateRot(Camera2D &cam, Player &p) {
    float widthRatio = (float)GetScreenWidth() / 320;
    float heightRatio = (float)GetScreenHeight() / 240;
    float scale = (widthRatio < heightRatio) ? widthRatio : heightRatio;

    Vector2 mouseScreenPos = GetMousePosition();

    Vector2 renderMouse = {(mouseScreenPos.x / scale),
                           (mouseScreenPos.y / scale)};

    Vector2 mousePos = GetScreenToWorld2D(renderMouse, cam);
    mouse = mousePos;

    Vector2 pCenter = {(float)p.x + p.w / 2.0f, (float)p.y + p.w / 2.0f};

    Vector2 delta = Vector2Subtract(pCenter, mousePos);

    float angle = atan2f(delta.y, delta.x) * RAD2DEG;
    rot = std::fmod(angle + 360.0f, 360.0f);
  }

  void focusOn(int cX, int cY, int r) {
    x = cX;
    y = cY;
    this->radius = r;
  }
};
