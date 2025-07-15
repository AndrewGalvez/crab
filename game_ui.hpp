#pragma once
#include "game_assets.hpp"
#include "game_runner.hpp"
#include "raylib.h"
#include <cmath>

class GameUI {
  GameRunner *runner;
  const Color SAND_COLOR = {0, 0, 0, 0};

public:
  void setRunner(GameRunner *runner) { this->runner = runner; };

  void drawBg(GameAssets *assets, int frame) {
    Shader *grainShader = assets->fetchShader("grain");
    int timeLoc = GetShaderLocation(*grainShader, "time");
    int resLoc = GetShaderLocation(*grainShader, "resolution");

    float time = floor(frame * 5);
    Vector2 res = {320, 240};

    SetShaderValue(*grainShader, timeLoc, &time, SHADER_UNIFORM_FLOAT);
    SetShaderValue(*grainShader, resLoc, &res, SHADER_UNIFORM_VEC2);

    BeginShaderMode(*grainShader);

    DrawRectangle(240, 0, 80, 240, BLUE);
    EndShaderMode();
  }

  void draw(GameAssets *assets, int frame) { drawBg(assets, frame); };
};
