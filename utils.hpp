#pragma once
#include "raylib.h"
#include <cmath>

inline void DrawRenderTexture(RenderTexture2D target, int targetWidth,
                              int targetHeight) {
  int screenWidth = GetScreenWidth();
  int screenHeight = GetScreenHeight();
  float scale = fminf((float)screenWidth / targetWidth,
                      (float)screenHeight / targetHeight);
  int renderWidth = (int)(targetWidth * scale);
  int renderHeight = (int)(targetHeight * scale);
  int offsetX = (screenWidth - renderWidth) / 2;
  int offsetY = (screenHeight - renderHeight) / 2;

  DrawTexturePro(
      target.texture,
      {0, 0, (float)target.texture.width, -(float)target.texture.height},
      {(float)offsetX, (float)offsetY, (float)renderWidth, (float)renderHeight},
      {0, 0}, 0, WHITE);
}

inline int dist(Vector2 a, Vector2 b) {
  int c = a.x - b.x;
  int d = a.y - b.y;

  return (c * c + d * d);
}
