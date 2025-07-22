#pragma once
#include "game_assets.hpp"
#include "game_runner.hpp"
#include "game_settings.hpp"
#include "game_state.hpp"
#include "game_ui.hpp"
#include "menus.hpp"
#include "music.hpp"

class Game {
private:
  GameSettings settings;

  GameUI ui;
  GameState state;
  GameAssets assets;
  GameRunner runner;
  MainMenu main_menu;
  SettingsMenu settings_menu;

  MusicManager m_manager;

  int frame = 0;
  int frameIncrementTimer = 0;
  int freezeFrames = 0;

public:
  Game() {
    settings.loadFromFile("data/settings");
    ui.setRunner(&this->runner);
    state = GAME_STATE_MAIN_MENU;
    assets.loadAssets();
    if (settings.musicEnabled) {
      m_manager.swapTrack("menu");
      m_manager.currentTrack.startMusic(&assets);
    }
    settings_menu.music_enabled_checkbox.checked = settings.musicEnabled;
  }

  void draw() {
    switch (state) {
    case GAME_STATE_MAIN_MENU:
      main_menu.draw(&assets, frameIncrementTimer);
      break;
    case GAME_STATE_SETTINGS:
      settings_menu.draw(&assets, frameIncrementTimer);
      break;
    case GAME_STATE_IN_GAME:
      runner.draw(&assets, frameIncrementTimer, freezeFrames);
      ui.draw(&assets, frameIncrementTimer);
      break;
    case GAME_STATE_SHOP:
      break;
    case GAME_STATE_DEAD:
      break;
    }
  };

  void update(bool *should_exit) {
    frame++;
    if (frame > 12) {
      frameIncrementTimer++;
      frame = 0;
    }
    if (frameIncrementTimer > 3) {
      frameIncrementTimer = 0;
    }

    if (settings.musicEnabled)
      m_manager.currentTrack.updateMusic(&assets);

    GameState last_state = state;

    switch (state) {
    case GAME_STATE_MAIN_MENU:
      main_menu.update(&assets, &state, &should_exit);
      break;
    case GAME_STATE_IN_GAME:
      if (freezeFrames > 0) {
        freezeFrames--;
        return;
      }
      runner.update(assets, freezeFrames, state);
      ui.update();
      break;
    case GAME_STATE_SETTINGS:
      settings_menu.update(&assets, &state, &settings);
      break;
    case GAME_STATE_DEAD:
      break;
    case GAME_STATE_SHOP:
      break;
    }

    if (last_state != GAME_STATE_IN_GAME && state == GAME_STATE_IN_GAME)
      runner.startLevel();

    if (last_state != state && settings.musicEnabled) {
      if (last_state == GAME_STATE_IN_GAME) {
        m_manager.currentTrack.stopMusic(&assets);
        m_manager.swapTrack("menu");
        m_manager.currentTrack.startMusic(&assets);
      } else if (state == GAME_STATE_IN_GAME) {
        m_manager.currentTrack.stopMusic(&assets);
        m_manager.swapTrack("ingame");
        m_manager.currentTrack.startMusic(&assets);
      }
    }
  };
};
