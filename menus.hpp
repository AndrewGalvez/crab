#pragma once
#include "game_assets.hpp"
#include "game_settings.hpp"
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

  GUIButton settings_button =
      GUIButton(165, 80, 150, 40, GRAY, DARKGRAY, false,
                GUIText(240, 90, 24, "Settings", BLACK, true));

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
    settings_button.draw();
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
    if (settings_button.isMouseOn() &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      *state = GAME_STATE_SETTINGS;
      PlaySound(*assets->fetchSound("select"));
    }
  };
};

class SettingsMenu {
private:
  GUIText title_text = GUIText(200, 10, 32, "Settings", WHITE, true);

  GUIButton back_button =
      GUIButton(10, 10, 100, 30, GRAY, DARKGRAY, false,
                GUIText(60, 15, 25, "Back", BLACK, true), true);

  GUIButton save_button =
      GUIButton(10, 190, 100, 30, GRAY, DARKGRAY, false,
                GUIText(60, 195, 25, "Save", BLACK, true), true);

  int saveMessageTimer = 0;

  GUIText save_message_text = GUIText(60, 165, 25, "Saved :)", GREEN, true);

  GUIText music_enabled_text =
      GUIText(10, 50, 25, "Music Enabled: ", WHITE, false);

public:
  GUICheckbox music_enabled_checkbox = GUICheckbox(200, 57, 10, 10, false);
  void draw(GameAssets *assets, int frame) {
    title_text.draw();
    if (saveMessageTimer > 0)
      save_message_text.draw();
    music_enabled_text.draw();
    music_enabled_checkbox.draw();

    // buttons

    Shader *grainShader = assets->fetchShader("grain");
    int timeLoc = GetShaderLocation(*grainShader, "time");
    int resLoc = GetShaderLocation(*grainShader, "resolution");

    float time = floor(GetTime() * 9);
    Vector2 res = {320, 240};

    SetShaderValue(*grainShader, timeLoc, &time, SHADER_UNIFORM_FLOAT);
    SetShaderValue(*grainShader, resLoc, &res, SHADER_UNIFORM_VEC2);

    BeginShaderMode(*grainShader);
    back_button.draw();
    save_button.draw();
    EndShaderMode();
  }

  void update(GameAssets *assets, GameState *state, GameSettings *settings) {
    if (saveMessageTimer > 0)
      saveMessageTimer--;

    if (music_enabled_checkbox.isMouseOn() &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      music_enabled_checkbox.tick();
      settings->musicEnabled = music_enabled_checkbox.checked;
    }

    if (back_button.isMouseOn() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      PlaySound(*assets->fetchSound("select"));
      *state = GAME_STATE_MAIN_MENU;
    }
    if (save_button.isMouseOn() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      PlaySound(*assets->fetchSound("select"));
      saveMessageTimer = 60;
      settings->saveToFile("data/settings");
    }
  }
};
