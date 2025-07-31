#pragma once
#include "game_assets.hpp"
#include "game_runner.hpp"
#include "gui.hpp"
#include "inventory.hpp"
#include "upgrades.hpp"
#include <cmath>
#include <memory.h>
#include <raylib.h>
#include <string>

struct CurrentUpgrade {
public:
  std::string index;
  std::string texid;

  GUIText name_text = GUIText(240, 130, 18, "Gold Multiplier", WHITE, true);
};

class ShopMenu {
private:
  GUIButton finish_button =
      GUIButton(10, 10, 100, 40, GRAY, DARKGRAY, false,
                GUIText(60, 20, 30, "Finish", BLACK, true), true);

  GUIButton main_menu_button =
      GUIButton(10, 55, 100, 40, GRAY, DARKGRAY, false,
                GUIText(60, 65, 30, "Menu", BLACK, true), true);

  GUIText shop_text = GUIText(240, 10, 48, "Shop", WHITE, true);

  GUIText gold_text = GUIText(140, 110, 16, "0", WHITE, false);

  GUIText hpotions_text = GUIText(140, 130, 16, "0", WHITE, false);

  GUIButton button_left = GUIButton(
      240 - 32 - 30, 60 + 32 - 12, 24, 24, GRAY, DARKGRAY, false,
      GUIText(240 - 32 - 30 + 12, 60 + 33 - 12, 24, "([", BLACK, true), false);

  GUIButton button_right = GUIButton(
      240 + 32 + 6, 60 + 32 - 12, 24, 24, GRAY, DARKGRAY, false,
      GUIText(240 + 32 + 6 + 12, 60 + 33 - 12, 24, "])", BLACK, true), false);

  CurrentUpgrade current_upgrade;

  GameRunner *runner;

  std::string gtxt;
  std::string hptxt;

public:
  ShopMenu() {
    current_upgrade.index = "goldmultiplier";
    current_upgrade.texid = "u_goldmultiplier";
  }

  void setRunner(GameRunner *runner) { this->runner = runner; }

  void draw_health_bar() {
    int x = 15;
    int y = 205;
    int w = 90;
    int h = 10;
    int padding = 2;

    DrawRectangle(x, y, w, h, BLACK);
    DrawRectangle(x + padding, y + padding, w - padding * 2, h - padding * 2,
                  RED);
    DrawRectangle(x + padding, y + padding,
                  w - padding * 2 * (runner->p.health / 100), h - padding * 2,
                  GREEN);
  };

  void draw_player_stuff(GameAssets &assets, int frame) {
    DrawTexturePro(
        *assets.fetchTexture("crab"), {(float)16 * frame, (float)0.0f, 16, 16},
        {(float)15, (float)110, (float)90, (float)90}, {0, 0}, 0, RAYWHITE);
    draw_health_bar();
  }

  void draw_inventory(GameAssets &assets) {
    gold_text.draw();
    DrawTexturePro(*assets.fetchTexture("gold"), {0, 0, 8, 8},
                   {120, 110, 16, 16}, {0, 0}, 0.0f, WHITE);

    hpotions_text.draw();
    DrawTexturePro(*assets.fetchTexture("hpotion"), {0, 0, 8, 8},
                   {120, 130, 16, 16}, {0, 0}, 0.0f, WHITE);
  }

  void draw_upgrades(GameAssets &assets, Upgrades &upgrades) {
    current_upgrade.name_text.draw();
    button_left.draw();
    button_right.draw();
    Texture2D *tex = assets.fetchTexture(current_upgrade.texid.c_str());

    DrawTexturePro(*tex, {0, 0, 16, 16}, {240 - 32, 60, 64, 64}, {0, 0}, 0.0f,
                   WHITE);
  }

  void draw(GameAssets &assets, int frame, Upgrades &upgrades) {
    Shader *grainShader = assets.fetchShader("grain");
    int timeLoc = GetShaderLocation(*grainShader, "time");
    int resLoc = GetShaderLocation(*grainShader, "resolution");

    float time = floor(GetTime() * 9);
    Vector2 res = {320, 240};

    SetShaderValue(*grainShader, timeLoc, &time, SHADER_UNIFORM_FLOAT);
    SetShaderValue(*grainShader, resLoc, &res, SHADER_UNIFORM_VEC2);

    BeginShaderMode(*grainShader);

    finish_button.draw();
    main_menu_button.draw();
    DrawRectangle(10, 110, 100, 115, DARKBLUE);
    EndShaderMode();

    shop_text.draw();
    draw_upgrades(assets, upgrades);

    draw_player_stuff(assets, frame);
    draw_inventory(assets);
  };

  void update(GameAssets *assets, Inventory &inv, Upgrades &upgrades) {
    gtxt = std::to_string(inv.getGold());
    gold_text.setValue(gtxt.c_str());
    hptxt = std::to_string(inv.gethPotions());
    hpotions_text.setValue(hptxt.c_str());

    Upgrade *u = upgrades.get(current_upgrade.index);

    if (!u)
      return;

    static std::string untxt;
    untxt = u->getName();
    current_upgrade.name_text.setValue(untxt.c_str());

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      if (button_left.isMouseOn() || button_right.isMouseOn()) {
        PlaySound(*assets->fetchSound("select"));

        Upgrade *un = nullptr;

        if (button_left.isMouseOn())
          un = upgrades.get(upgrades.getPrev(current_upgrade.index));
        else
          un = upgrades.get(upgrades.getNext(current_upgrade.index));

        if (!un) {
          return;
        }

        current_upgrade.index = un->getId();
        current_upgrade.texid = un->getTexID();
        current_upgrade.name_text.setValue(un->getName().c_str());
      }
    }
  }
};
