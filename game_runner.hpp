#pragma once

#include "bullet.hpp"
#include "enemy.hpp"
#include "game_assets.hpp"
#include "game_map.hpp"
#include "game_state.hpp"
#include "gui.hpp"
#include "gun.hpp"
#include "raylib.h"
#include <cassert>
#include <cstdlib>
#include <vector>

class GameRunner {
private:
  std::vector<Enemy> enemies = {Enemy(100, 100)};
  GameMap map;
  Camera2D cam;

  GUIText finishedText =
      GUIText(240 / 2, 25, 22, "Level finished!", GREEN, true);
  GUIText finishedText2 =
      GUIText(240 / 2, 55, 18, "Return to shop: Q", GREEN, true);

public:
  Player p = Player(10, 10, 32, 32);
  Gun currentGun;
  std::vector<Bullet> bullets;
  int level = 0;
  bool levelFinished = false;

  GameRunner() {
    cam.offset = {(float)240 / 2, (float)240 / 2};
    cam.rotation = 0.0f;
    cam.zoom = 1.0f;
    map.loadFromFile("data/map");

    p.findSpawn(map);

    p.update(0, map.size, map);
  }

  void startLevel() {
    level++;
    levelFinished = false;

    for (int i = 0; i < level; i++) {
      int tX = 0;
      int tY = 0;

      while (map.get(tX, tY)) {
        srand(time(0));
        tX = (int)(rand() % 18) + 1;
        tY = (int)(rand() % 18) + 1;
      }

      enemies.push_back(Enemy(tX * 32, tY * 32));
    }
  }

  void draw(GameAssets *assets, int frame, int freezeFrames) {
    BeginMode2D(cam);
    map.draw();
    p.draw(assets->fetchTexture("crab"), frame, &cam, map.size, freezeFrames,
           assets->fetchShader("whitemask"));
    currentGun.draw();
    for (Enemy &e : enemies) {
      e.draw(assets, frame);
    }
    for (Bullet &b : bullets) {
      b.draw();
    }
    EndMode2D();

    if (levelFinished) {
      finishedText.draw();
      finishedText2.draw();
    }
  };

  void shoot(float speed) {
    float angleRad = -currentGun.rot * DEG2RAD;
    Vector2 dir = Vector2Scale({cosf(angleRad), -sinf(angleRad)}, -speed);
    Vector2 sOffset = Vector2Scale({cosf(angleRad), -sinf(angleRad)}, -40);
    Vector2 origin = {(float)p.x + p.w / 2.0f, (float)p.y + p.h / 2.0f};
    Vector2 spawnPos = Vector2Add(origin, sOffset);
    bullets.push_back(Bullet(spawnPos.x, spawnPos.y, 2, dir));
  }

  void update(GameAssets &assets, int &freezeFrames, GameState &state) {
    float dt = GetFrameTime();

    p.update(dt, map.size, map);
    currentGun.focusOn(p.x + p.w / 2, p.y + p.w / 2, 40);
    currentGun.updateRot(cam, p);

    /* enemies and bullets */ {
      std::vector<int> to_erase;

      for (int i = 0; i < enemies.size(); i++) {
        Enemy &e = enemies.at(i);
        e.update(dt, p, map.tilemap);
        if (CheckCollisionRecs(
                {(float)p.x, (float)p.y, (float)p.w, (float)p.h},
                {e.getX(), e.getY(), (float)e.getW(), (float)e.getH()})) {
          p.health -= 25;
          freezeFrames = 15;
          PlaySound(*assets.fetchSound("hurt"));
          to_erase.push_back(i);
        }
      }

      std::vector<int> to_remove;

      for (int i = 0; i < bullets.size(); i++) {
        Bullet &b = bullets[i];
        b.move();
        int bx = b.x / map.TILE_SIZE;
        int by = b.y / map.TILE_SIZE;
        if (map.get(bx, by)) {
          to_remove.push_back(i);
          continue;
        }

        for (int j = 0; j < enemies.size(); j++) {
          Enemy &e = enemies[j];
          if (b.x > e.getX() && b.x < e.getX() + e.getW() && b.y > e.getY() &&
              b.y < e.getY() + e.getH()) {
            to_erase.push_back(j);
            to_remove.push_back(i);
            PlaySound(*assets.fetchSound("hit"));
          }
        }
      }

      for (int i : to_erase) {

        enemies.erase(enemies.begin() + i);
      }

      for (int j : to_remove) {
        bullets.erase(bullets.begin() + j);
      }
    }

    if (currentGun.cooldown == 0) {
      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        shoot(currentGun.bSpeed);
        currentGun.cooldown = currentGun.baseCooldown;
        PlaySound(*assets.fetchSound("shoot"));
      }
    } else {
      currentGun.cooldown--;
    }

    // level finish

    if (enemies.size() == 0 && !levelFinished) {
      levelFinished = true;
    }
  }
};
