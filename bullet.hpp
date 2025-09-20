#pragma once
#include "raylib.h"
#include <raymath.h>

class Bullet {
public:
  int x;
  int y;
  int r;
  float distTraveled = 0;
  unsigned int ricochets = 0;
  Vector2 dir;
  Color c = PURPLE;

  Bullet(int x, int y, int r, Vector2 dir, unsigned int ricochets)
      : x(x), y(y), r(r), dir(dir), ricochets(ricochets) {}

  void draw() { DrawCircle(x, y, r, c); }

  void move() {
    Vector2 c = {(float)x, (float)y};
    x += dir.x;
    y += dir.y;
    Vector2 n = {(float)x, (float)y};
    distTraveled += Vector2Distance(c, n);
  }
};
