#pragma once
#include "game_assets.hpp"
#include "game_map.hpp"
#include "game_runner.hpp"
#include "gold_manager.hpp"
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

public:
  void setRunner(GameRunner *runner) { this->runner = runner; };

  void drawBg(GameAssets *assets, int frame) {
    Shader *grainShader = assets->fetchShader("grain");
    int timeLoc = GetShaderLocation(*grainShader, "time");
    int resLoc = GetShaderLocation(*grainShader, "resolution");

    float time = floor(frame * 5);
    Vector2 res = {320, 240};

    SetShaderValue(*grainShader, timeLoc, &time, SHADER_UNIFORM_FLOAT);
    SetShaderValue(*grainShader, resLoc, &res, SHADER_UNIFORM_VEC2);

    BeginShaderMode(*grainShader);

    DrawRectangle(240, 0, 80, 240, BLUE);
    EndShaderMode();
  }

  void draw_health_bar() {
    DrawRectangle(250, 40, 20, 108, BLACK);
    DrawRectangle(254, 44, 12, 100, RED);
    DrawRectangle(254, 44, 12, runner->p.health, GREEN);
    health_text.draw();
  };

  void draw_gun_cooldown() {
    DrawRectangle(275, 40, 40, 10, BLACK);

    if (runner->currentGun.magBullets != 0) {
      int barHeight = 8;
      int startX = 277;
      int maxWidth = 36;
      int margin = 1;

      int totalBars = runner->currentGun.magSizeMax;
      int totalMargins = (totalBars - 1) * margin;
      int availableWidth = maxWidth - totalMargins;
      int barWidth = availableWidth / totalBars;

      for (int i = 0; i < runner->currentGun.magBullets; i++) {
        int x = startX + (i * (barWidth + margin));
        DrawRectangle(x, 41, barWidth, barHeight, GOLD);
      }
    } else {
      DrawRectangle(277, 41,
                    runner->currentGun.reloadFrameCurrent /
                        runner->currentGun.reloadFrames * 36,
                    8, RED);
    }
  }

  void draw_minimap(GameMap &map) {
    DrawRectangle(250, 160, 60, 60, DARKBLUE);

    for (int col = 0; col < map.tile_num; col++) {
      for (int row = 0; row < map.tile_num; row++) {
        if (map.renderer[row][col].tx != -1)
          DrawRectangle(250 + col * 3, 160 + row * 3, 3, 3, WHITE);
      }
    }

    DrawRectangle(runner->p.x / map.TILE_SIZE * 3 + 250,
                  runner->p.y / map.TILE_SIZE * 3 + 160, 3, 3, RED);

    for (Gold &g : runner->gold_manager.golds) {
      DrawRectangle((int)g.getRect().x / map.TILE_SIZE * 3 + 250,
                    (int)g.getRect().y / map.TILE_SIZE * 3 + 160, 3, 3, GOLD);
    }
  };

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

  void draw(GameAssets *assets, int frame, GameMap &map) {
    drawBg(assets, frame);
    level_text.draw();
    draw_health_bar();
    draw_gun_cooldown();
    gold_text.draw();
    healthpotions_text.draw();
    draw_minimap(map);
    DrawTexturePro(*assets->fetchTexture("gold"), {0, 0, 8, 8},
                   {275, 55, 16, 16}, {0, 0}, 0.0f, WHITE);
    DrawTexturePro(*assets->fetchTexture("hpotion"), {0, 0, 8, 8},
                   {275, 75, 16, 16}, {0, 0}, 0.0f, WHITE);
  };
};
