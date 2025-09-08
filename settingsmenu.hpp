#pragma once
#include "game_assets.hpp"
#include "game_settings.hpp"
#include "game_state.hpp"
#include "gui.hpp"
#include "raylib.h"
#include "sound.hpp"

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

  GUIText sound_enabled_text =
      GUIText(10, 80, 25, "Sound Enabled: ", WHITE, false);

public:
  GUICheckbox music_enabled_checkbox = GUICheckbox(200, 57, 10, 10, false);
  GUICheckbox sound_enabled_checkbox = GUICheckbox(200, 87, 10, 10, false);

  void draw(GameAssets *assets, int frame) {
    title_text.draw();
    if (saveMessageTimer > 0)
      save_message_text.draw();
    music_enabled_text.draw();
    music_enabled_checkbox.draw();

    sound_enabled_text.draw();
    sound_enabled_checkbox.draw();

    // buttons

    float time = floor(GetTime() * 9);
    Vector2 res = {320, 240};
    assets->setShaderValue("grain", "time", time);
    assets->setShaderValue("grain", "resolution", res);

    assets->beginShaderMode("grain");
    back_button.draw();
    save_button.draw();
    assets->endShaderMode("grain");
  }

  void update(SoundManager &s_manager, GameState *state,
              GameSettings *settings) {
    if (saveMessageTimer > 0)
      saveMessageTimer--;

    if (music_enabled_checkbox.isMouseOn() &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      music_enabled_checkbox.tick();
      settings->musicEnabled = music_enabled_checkbox.checked;
    }

    if (sound_enabled_checkbox.isMouseOn() &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      sound_enabled_checkbox.tick();
      settings->soundEnabled = sound_enabled_checkbox.checked;
    }

    if (back_button.isMouseOn() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      s_manager.play("select");
      *state = GAME_STATE_MAIN_MENU;
    }

    if (save_button.isMouseOn() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      s_manager.play("select");
      saveMessageTimer = 60;
      settings->saveToFile("data/settings");
    }
  }
};
