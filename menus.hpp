#pragma once
#include "game_assets.hpp"
#include "game_state.hpp"
#include "gui.hpp"
#include "raylib.h"

class MainMenu {
private:
  GUIText title_text =
      GUIText(10, 10, 48, (const char *)"Crab (2)", WHITE, false);

  GUIText credit_text = GUIText(
      320 / 2, 220, 16, (const char *)"Music: Fesliyan Studios - David Renda",
      WHITE, true);

  GUIText develop_text =
      GUIText(25, 50, 20, (const char *)"by Andy Galvez", WHITE, false);

  GUIButton start_button =
      GUIButton(10, 80, 150, 40, GRAY, DARKGRAY, false,
                GUIText(85, 90, 24, "Start Game", BLACK, true), true);
  GUIButton quit_button =
      GUIButton(10, 130, 150, 40, GRAY, DARKGRAY, false,
                GUIText(85, 140, 24, "Quit Game", BLACK, true));

public:
  void draw(GameAssets *game_assets, int frame) {
    Shader *grainShader = game_assets->fetchShader("grain");
    int timeLoc = GetShaderLocation(*grainShader, "time");
    int resLoc = GetShaderLocation(*grainShader, "resolution");

    float time = floor(GetTime() * 9);
    Vector2 res = {320, 240};

    SetShaderValue(*grainShader, timeLoc, &time, SHADER_UNIFORM_FLOAT);
    SetShaderValue(*grainShader, resLoc, &res, SHADER_UNIFORM_VEC2);

    BeginShaderMode(*grainShader);
    start_button.draw();
    quit_button.draw();
    EndShaderMode();
    title_text.draw();
    develop_text.draw();
    credit_text.draw();
    DrawTexturePro(*game_assets->fetchTexture("crab"),
                   {16 * (float)(frame % 4), 0, 16, 16},
                   {225, 0, 16 * 4, 16 * 4}, {0, 0}, 0.0f, WHITE);
  };

  void update(GameAssets *assets, GameState *state, bool **should_exit) {
    if (start_button.isMouseOn() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      *state = GAME_STATE_IN_GAME;
      PlaySound(*assets->fetchSound("select"));
    }
    if (quit_button.isMouseOn() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      **should_exit = true;
      PlaySound(*assets->fetchSound("select"));
    }
  };
};
