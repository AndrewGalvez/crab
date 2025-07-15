#pragma once
#include "raylib.h"

class Player {
private:
  int x;
  int y;
  int w;
  int h;
  int speed = 50;
  int animY = 0;

public:
  Player(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {}

  void draw(Texture2D *tex, int frame) {
    DrawTexturePro(*tex, {(float)16 * frame, (float)animY, 16, 16},
                   {(float)x, (float)y, (float)w, (float)h}, {0, 0}, 0, WHITE);
  };

  void update(float dt) {
    int moveSpeed = this->speed * dt;
    int prevX = x;
    int prevY = y;
    this->animY = 0;

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
      this->x -= moveSpeed;
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
      this->y -= moveSpeed;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
      this->x += moveSpeed;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
      this->y += moveSpeed;

    if (this->x != prevX || this->y != prevY) {
      animY = 16;
    }
  }
};
