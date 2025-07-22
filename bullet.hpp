#pragma once
#include "raylib.h"

class Bullet {
public:
  int x;
  int y;
  int r;
  Vector2 dir;
  Color c = PURPLE;

  Bullet(int x, int y, int r, Vector2 dir) : x(x), y(y), r(r), dir(dir) {}

  void draw() { DrawCircle(x, y, r, c); }

  void move() {
    x += dir.x;
    y += dir.y;
  }
};
