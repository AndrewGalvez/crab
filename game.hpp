#pragma once
#include "game_assets.hpp"
#include "game_runner.hpp"
#include "game_state.hpp"
#include "game_ui.hpp"
#include "menus.hpp"
#include "music.hpp"

class Game {
private:
  GameUI ui;
  GameState state;
  GameAssets assets;
  GameRunner runner;
  MainMenu main_menu;

  MusicManager m_manager;

  int frame = 0;
  int frameIncrementTimer = 0;

public:
  Game() {
    ui.setRunner(&this->runner);
    state = GAME_STATE_MAIN_MENU;
    assets.loadAssets();
    m_manager.swapTrack("menu");
    m_manager.currentTrack.startMusic(&assets);
  }

  void draw() {
    switch (state) {
    case GAME_STATE_MAIN_MENU:
      main_menu.draw(&assets, frameIncrementTimer);
      break;
    case GAME_STATE_IN_GAME:
      ui.draw(&assets, frameIncrementTimer);
      runner.draw(&assets, frameIncrementTimer);
      break;
    case GAME_STATE_SHOP:
      break;
    case GAME_STATE_DEAD:
      break;
    }
  };

  void update(bool *should_exit) {
    frame++;
    if (frame > 4) {
      frameIncrementTimer++;
      frame = 0;
    }
    if (frameIncrementTimer > 31) {
      frameIncrementTimer = 0;
    }

    m_manager.currentTrack.updateMusic(&assets);

    GameState last_state = state;

    switch (state) {
    case GAME_STATE_MAIN_MENU:
      main_menu.update(&assets, &state, &should_exit);
      break;
    case GAME_STATE_IN_GAME:
      runner.update();
      break;
    case GAME_STATE_DEAD:
      break;
    case GAME_STATE_SHOP:
      break;
    }

    if (last_state != state) {
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
