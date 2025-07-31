#pragma once
#include "game_assets.hpp"
#include "game_settings.hpp"
#include "game_state.hpp"
#include "gui.hpp"
#include "raylib.h"

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
