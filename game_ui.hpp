#pragma once
#include "game_assets.hpp"
#include "game_runner.hpp"
#include "gui.hpp"
#include "raylib.h"
#include <cmath>
#include <memory.h>
#include <string>

class GameUI {
  GameRunner *runner;
  GUIText level_text = GUIText(280, 10, 20, "Level X", WHITE, true);
  GUIText health_text = GUIText(260, 30, 10, "100", WHITE, true);
  GUIText gold_text = GUIText(275 + 20, 55, 16, "0", WHITE, false);
  GUIText healthpotions_text = GUIText(275 + 20, 75, 16, "0", WHITE, false);
  const Color SAND_COLOR = {0, 0, 0, 0};

public:
  void setRunner(GameRunner *runner) { this->runner = runner; };

  void drawBg(GameAssets *assets, int frame) {
    float time = floor(frame * 5);
    Vector2 res = {320, 240};
    assets->setShaderValue("grain", "time", time);
    assets->setShaderValue("grain", "resolution", res);

    assets->beginShaderMode("grain");
    DrawRectangle(240, 0, 80, 240, BLUE);
    assets->endShaderMode("grain");
  }

  void draw_health_bar() {
    DrawRectangle(250, 40, 20, 108, BLACK);
    DrawRectangle(254, 44, 12, 100, RED);
    DrawRectangle(254, 44, 12, runner->p.health, GREEN);
    health_text.draw();
  };

  void draw_gun_cooldown() {
    DrawRectangle(275, 40, 40, 10, BLACK);
    DrawRectangle(277, 42, 36, 6, GREEN);
    DrawRectangle(277, 42,
                  36.0f * ((float)runner->currentGun.cooldown /
                           runner->currentGun.baseCooldown),
                  6, RED);
  }

  void update() {
    std::string ltxt = "Level " + std::to_string(runner->level);
    level_text.setValue(strdup(ltxt.c_str()));
    std::string htxt = std::to_string(runner->p.health);
    health_text.setValue(strdup(htxt.c_str()));
    std::string gtxt = std::to_string(runner->inv.getGold());
    gold_text.setValue(strdup(gtxt.c_str()));
    std::string hptxt = std::to_string(runner->inv.gethPotions()) + "\n(Z)";
    healthpotions_text.setValue(strdup(hptxt.c_str()));
  }

  void draw(GameAssets *assets, int frame) {
    drawBg(assets, frame);
    level_text.draw();
    draw_health_bar();
    draw_gun_cooldown();
    gold_text.draw();
    healthpotions_text.draw();
    DrawTexturePro(*assets->fetchTexture("gold"), {0, 0, 8, 8},
                   {275, 55, 16, 16}, {0, 0}, 0.0f, WHITE);
    DrawTexturePro(*assets->fetchTexture("hpotion"), {0, 0, 8, 8},
                   {275, 75, 16, 16}, {0, 0}, 0.0f, WHITE);
  };
};
