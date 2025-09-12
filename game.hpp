#pragma once
#include "deadmenu.hpp"
#include "game_assets.hpp"
#include "game_runner.hpp"
#include "game_settings.hpp"
#include "game_state.hpp"
#include "game_ui.hpp"
#include "mainmenu.hpp"
#include "music.hpp"
#include "settingsmenu.hpp"
#include "shop.hpp"
#include "sound.hpp"
#include "upgrades.hpp"
#include <raylib.h>

class Game {
private:
  GameSettings settings;

  Inventory inventory;
  Upgrades upgrades;
  GameUI ui;
  GameState state;
  GameAssets assets;
  GameRunner runner = GameRunner(inventory);
  MainMenu main_menu;
  SettingsMenu settings_menu;
  ShopMenu shop_menu;
  DeadMenu dead_menu;

  MusicManager m_manager;
  SoundManager s_manager = SoundManager(true, &assets);

  int frame = 0;
  int frameIncrementTimer = 0;

public:
  Game() {
    settings.loadFromFile("data/settings");
    ui.setRunner(&this->runner);
    shop_menu.setRunner(&runner);
    state = GAME_STATE_MAIN_MENU;
    assets.loadAssets();
    if (settings.musicEnabled) {
      m_manager.currentTrack.startMusic(&assets);
    }
    s_manager.enabled = settings.soundEnabled;
    settings_menu.music_enabled_checkbox.checked = settings.musicEnabled;
    settings_menu.sound_enabled_checkbox.checked = settings.soundEnabled;
    upgrades.init();
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
      runner.draw(&assets, frameIncrementTimer, frame, upgrades);
      ui.draw(&assets, frameIncrementTimer);
      break;
    case GAME_STATE_SHOP:
      shop_menu.draw(assets, frameIncrementTimer, upgrades);
      break;
    case GAME_STATE_DEAD:
      dead_menu.draw(assets);
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
      main_menu.update(s_manager, &state, &should_exit);
      break;
    case GAME_STATE_IN_GAME:
      runner.update(s_manager, state, upgrades);
      ui.update();
      break;
    case GAME_STATE_SETTINGS:
      settings_menu.update(s_manager, &state, &settings);
      break;
    case GAME_STATE_DEAD:
      dead_menu.update(GetFrameTime(), s_manager, state, runner, upgrades,
                       inventory);
      break;
    case GAME_STATE_SHOP:
      shop_menu.update(s_manager, inventory, upgrades, state);
      break;
    }

    if (last_state != GAME_STATE_IN_GAME && state == GAME_STATE_IN_GAME)
      runner.startLevel(upgrades);

    if (last_state != state && settings.musicEnabled) {
      if (last_state == GAME_STATE_IN_GAME) {
        m_manager.currentTrack.stopMusic(&assets);
        m_manager.swapTrack("menu");
        if (GetMusicTimePlayed(m_manager.currentTrack.fetch(&assets)) > 0.0f)
          m_manager.currentTrack.resumeMusic(&assets);
        else
          m_manager.currentTrack.startMusic(&assets);
      } else if (state == GAME_STATE_IN_GAME) {
        m_manager.currentTrack.stopMusic(&assets);
        m_manager.swapTrack("ingame");
        if (GetMusicTimePlayed(m_manager.currentTrack.fetch(&assets)) > 0.0f)
          m_manager.currentTrack.resumeMusic(&assets);
        else
          m_manager.currentTrack.startMusic(&assets);
      }
    }
  };
};
