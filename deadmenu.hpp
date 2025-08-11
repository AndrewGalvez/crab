#pragma once
#include "game_assets.hpp"
#include "game_runner.hpp"
#include "game_state.hpp"
#include "gui.hpp"
#include "sound.hpp"
#include <raylib.h>

class DeadMenu {
private:
  GUIText header =
      GUIText(-100, 5, 32, "DEAD-DEAD-DEAD-DEAD-DEAD-DEAD-", RED, false);
  GUIText header2 = GUIText(5, 40, 24, "(skill issue)", RED, false);

  float headerscrollspeed = 1;
  float headerpos = -100;
  float headerresetx = headerpos + MeasureText("DEAD-", 32);

  GUIButton new_game_button = GUIButton(
      10, 100, 150, 40, GRAY, DARKGRAY, false,
      GUIText(10 + 150 / 2, 100 + 26 / 4, 26, "NEW GAME", BLACK, true), true);
  GUIButton main_menu_button = GUIButton(
      10, 150, 150, 40, GRAY, DARKGRAY, false,
      GUIText(10 + 150 / 2, 150 + 26 / 4, 26, "MAIN MENU", BLACK, true), true);

public:
  void draw(GameAssets &assets) {
    header.draw();
    header2.draw();

    Shader *grainShader = assets.fetchShader("grain");
    int timeLoc = GetShaderLocation(*grainShader, "time");
    int resLoc = GetShaderLocation(*grainShader, "resolution");

    float time = floor(GetTime() * 9);
    Vector2 res = {320, 240};

    SetShaderValue(*grainShader, timeLoc, &time, SHADER_UNIFORM_FLOAT);
    SetShaderValue(*grainShader, resLoc, &res, SHADER_UNIFORM_VEC2);

    BeginShaderMode(*grainShader);

    new_game_button.draw();
    main_menu_button.draw();

    EndShaderMode();
  }

  void update(float dt, SoundManager &s, GameState &state, GameRunner &r,
              Upgrades &u, Inventory &inv) {
    if (headerpos >= headerresetx) {
      headerpos = -100 - headerresetx;
      headerpos -= headerscrollspeed;
    }
    headerpos += headerscrollspeed;
    header.setPos(headerpos, 5);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      if (new_game_button.isMouseOn()) {
        r.reset();
        u.init();
        inv.reset();
        s.play("select");
        state = GAME_STATE_IN_GAME;
      }
      if (main_menu_button.isMouseOn()) {
        r.reset();
        u.init();
        inv.reset();
        s.play("select");
        state = GAME_STATE_MAIN_MENU;
      }
    }
  }
};
