#pragma once
#include "game_assets.hpp"
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

inline Vector2 GetScreenMousePos(Camera2D cam) {
  float widthRatio = (float)GetScreenWidth() / 320;
  float heightRatio = (float)GetScreenHeight() / 240;
  float scale = (widthRatio < heightRatio) ? widthRatio : heightRatio;
  int renderWidth = (int)(320 * scale);
  int renderHeight = (int)(240 * scale);
  int offsetX = (GetScreenWidth() - renderWidth) / 2;
  int offsetY = (GetScreenHeight() - renderHeight) / 2;
  Vector2 mouseScreenPos = GetMousePosition();

  Vector2 renderMouse = {(mouseScreenPos.x - offsetX) / scale,
                         (mouseScreenPos.y - offsetY) / scale};

  Vector2 mousePos = GetScreenToWorld2D(renderMouse, cam);

  return mousePos;
};

inline int dist(Vector2 a, Vector2 b) {
  int c = a.x - b.x;
  int d = a.y - b.y;

  return (c * c + d * d);
}

inline void grain(GameAssets &assets) {
  Shader *grainShader = assets.fetchShader("grain");
  int timeLoc = GetShaderLocation(*grainShader, "time");
  int resLoc = GetShaderLocation(*grainShader, "resolution");

  float time = floor(GetTime() * 9);
  Vector2 res = {320, 240};

  SetShaderValue(*grainShader, timeLoc, &time, SHADER_UNIFORM_FLOAT);
  SetShaderValue(*grainShader, resLoc, &res, SHADER_UNIFORM_VEC2);

  BeginShaderMode(*grainShader);
}
