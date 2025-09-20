#pragma once
#include "bullet.hpp"
#include "effect.hpp"
#include "enemy.hpp"
#include "game_map.hpp"
#include "game_state.hpp"
#include "gold_manager.hpp"
#include "gui.hpp"
#include "gun.hpp"
#include "inventory.hpp"
#include "raylib.h"
#include "sound.hpp"
#include "upgrades.hpp"
#include <cassert>
#include <raymath.h>
#include <vector>

class GameRunner {
private:
  std::vector<Enemy> enemies = {};
  std::vector<EnemyDeath> enemydeaths = {};
  std::vector<HealthPotionEffect> hpeffects = {};

  GameMap map = GameMap();
  Camera2D cam;
  Vector2 camoffsetbase;
  Vector2 screenShake = {0, 0};
  int freezeFrames = 0;
  bool dead = false;

  GUIText finishedText =
      GUIText(240 / 2, 25, 22, "Level finished!", GREEN, true);
  GUIText finishedText2 =
      GUIText(240 / 2, 55, 18, "Return to shop: Q", GREEN, true);

  void shoot(float speed, unsigned int ricochets, SoundManager &s) {
    if (currentGun.magBullets > 0) {
      currentGun.magBullets--;
      for (int i = 0; i < currentGun.bulletCount; i++) {
        float angleRad = -currentGun.rot * DEG2RAD +
                         (GetRandomValue(currentGun.spread * 1000,
                                         -currentGun.spread * 1000) /
                          1000.0f);
        float startAngle =
            angleRad + ((i - 1) * (((0 - currentGun.bulletDegrees) * 2) /
                                   currentGun.bulletCount));
        Vector2 dir =
            Vector2Scale({cosf(startAngle), -sinf(startAngle)}, -speed);
        Vector2 sOffset = Vector2Scale({cosf(angleRad), -sinf(angleRad)}, -40);
        Vector2 origin = {(float)p.x + p.w / 2.0f + currentGun.w / 2.0f,
                          (float)p.y + p.h / 2.0f + currentGun.w / 2.0f};
        Vector2 spawnPos = Vector2Add(origin, sOffset);
        bullets.push_back(Bullet(spawnPos.x, spawnPos.y, 2, dir, ricochets));
        s.play("shoot");
      }
    }
  }

  void update_gold(SoundManager &s, Upgrades &u) {
    std::vector<int> g = gold_manager.isGoldColliding(
        {(float)p.x, (float)p.y, (float)p.w, (float)p.h});

    for (int i : g) {
      inv.addGold(1);
      gold_manager.removeGold(i);
      s.play("pickupGold");
    }

    gold_manager.updateGoldMagnet(p.x + p.w / 2.0f, p.y + p.h / 2.0f,
                                  u.get("goldmagnet")->getCurrent());
  }

  void update_hpeffects() {
    for (HealthPotionEffect &hpe : hpeffects) {
      hpe.update();
    }
  }

  void update_enemies(float dt, SoundManager &s,
                      std::vector<int> &to_erase_enemies) {
    for (int i = 0; i < enemies.size(); i++) {
      Enemy &e = enemies.at(i);
      e.update(dt, p, map.tilemap);
      if (CheckCollisionRecs(
              {(float)p.x, (float)p.y, (float)p.w, (float)p.h},
              {e.getX(), e.getY(), (float)e.getW(), (float)e.getH()})) {
        p.health -= 25;
        s.play("hurt");
        freezeFrames = 15;
        to_erase_enemies.push_back(i);
      }
    }
  }

  void update_bullets(std::vector<int> &to_erase_enemies, SoundManager &s) {
    std::vector<int> to_remove;

    for (int i = 0; i < bullets.size(); i++) {
      Bullet &b = bullets[i];
      if (b.distTraveled > currentGun.bMaxDist &&
          currentGun.bMaxDist != -1.0f) {
        to_remove.push_back(i);
        continue;
      }
      int bx = b.x / map.TILE_SIZE;
      int by = b.y / map.TILE_SIZE;
      b.move();
      int bnx = b.x / map.TILE_SIZE;
      int bny = b.y / map.TILE_SIZE;

      if (map.get(bnx, bny)) {
        if (b.ricochets <= 0) {
          to_remove.push_back(i);
          continue;
        }
        b.ricochets--;
        if (!map.get(bx, bny)) {
          b.dir.x *= -1;
        } else if (!map.get(bnx, by)) {
          b.dir.y *= -1;
        } else {
          b.dir.x *= -1;
          b.dir.y *= -1;
        }
        continue;
      }

      for (int j = 0; j < enemies.size(); j++) {
        Enemy &e = enemies[j];
        if (b.x > e.getX() && b.x < e.getX() + e.getW() && b.y > e.getY() &&
            b.y < e.getY() + e.getH()) {
          e.health -= currentGun.dmg;
          e.vel = Vector2Add(e.vel, Vector2Normalize(b.dir) *
                                        currentGun.knockbackForce);
          if (e.health <= 0.0f)
            to_erase_enemies.push_back(j);
          to_remove.push_back(i);
          s.play("hit");
        }
      }
    }

    std::sort(to_remove.begin(), to_remove.end(), std::greater<int>());
    to_remove.erase(std::unique(to_remove.begin(), to_remove.end()),
                    to_remove.end());

    for (int i : to_remove) {
      bullets.erase(bullets.begin() + i);
    }
  }

  void update_enemy_deaths() {
    std::vector<int> to_delete;

    for (int i = 0; i < enemydeaths.size(); i++) {
      EnemyDeath &ed = enemydeaths[i];
      if (ed.dead()) {
        to_delete.push_back(i);
      }
    }

    std::sort(to_delete.begin(), to_delete.end(), std::greater<int>());
    to_delete.erase(std::unique(to_delete.begin(), to_delete.end()),
                    to_delete.end());

    for (int i : to_delete) {
      enemydeaths.erase(enemydeaths.begin() + i);
    }
  };

  void update_player(SoundManager &s, float dt) {
    p.update(dt, map.size, map);
    currentGun.focusOn(p.x + p.w / 2, p.y + p.w / 2, 40);
    currentGun.updateRot(cam, p);

    if (currentGun.cooldown == 0) {
      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        shoot(currentGun.bSpeed, currentGun.bRicochets, s);
        currentGun.cooldown = currentGun.baseCooldown;
      }
    } else {
      currentGun.cooldown--;
    }

    if (currentGun.magBullets == 0) {
      currentGun.reloadFrameCurrent--;
      if (currentGun.reloadFrameCurrent <= 0) {
        currentGun.magBullets = currentGun.magSizeMax;
        currentGun.reloadFrameCurrent = currentGun.reloadFrames;
      }
    }
  }

  void update_hpotions(SoundManager &s) {
    if (IsKeyPressed(KEY_Z) && inv.gethPotions() > 0 && p.health < 100) {
      p.health += 50;
      if (p.health > 100)
        p.health = 100;

      inv.removeHPotions(1);
      s.play("hPotion");
      hpeffects.push_back(HealthPotionEffect(p.x + p.w / 2, p.y + p.h / 2));
    }
  }

public:
  Player p = Player(10, 10, 32, 32);
  std::array<Gun, 3> all_guns = {Gun(), LMG(), Shotgun()};
  Gun currentGun = all_guns[0];
  GoldManager gold_manager;
  std::vector<Bullet> bullets;
  int level = 0;
  bool levelFinished = false;
  Inventory &inv;

  GameRunner(Inventory &inv) : inv(inv) {
    camoffsetbase = {(float)240 / 2, (float)240 / 2};
    cam.offset = {(float)240 / 2, (float)240 / 2};
    cam.rotation = 0.0f;
    cam.zoom = 1.0f;
    map.loadFromFile("data/map");
    map.loadRenderFromFile("data/maprender");

    p.findSpawn(map);

    p.update(0, map.size, map);
  }

  GameMap &getMapRef() { return map; }

  void reset() {
    p = Player(10, 10, 32, 32);
    currentGun = Gun();
    gold_manager = GoldManager();
    bullets.clear();
    level = 0;
    levelFinished = false;
    dead = false;

    camoffsetbase = {(float)240 / 2, (float)240 / 2};
    cam.offset = {(float)240 / 2, (float)240 / 2};
    cam.rotation = 0.0f;
    cam.zoom = 1.0f;
    map.loadFromFile("data/map");

    p.findSpawn(map);

    p.update(0, map.size, map);
  }

  void startLevel(Upgrades &u) {
    currentGun.magBullets = currentGun.magSizeMax;
    currentGun.reloadFrameCurrent = currentGun.reloadFrames;
    level++;
    levelFinished = false;

    for (int i = 0; i < level; i++) {
      int tX = 0;
      int tY = 0;

      int maxtries = 1000;
      bool enemyDistGood = true;
      for (int i = 0; i < maxtries; i++) {
        srand(time(0));
        tX = GetRandomValue(1, 18);
        tY = GetRandomValue(1, 18);
        if (!map.get(tX, tY))
          if (Vector2Distance({tX * 32.0f, tY * 32.0f},
                              {(float)p.x, (float)p.y}) > 32 * 3) {
            for (Enemy &e : enemies) {
              if (Vector2Distance({e.getX(), e.getY()},
                                  {tX * 32.0f, tY * 32.0f}) < 32 * 3) {
                enemyDistGood = false;
              }
            }
            if (enemyDistGood)
              break;
          }
      }
      while (!enemyDistGood) {
        srand(time(0));
        tX = GetRandomValue(1, 18);
        tY = GetRandomValue(1, 18);
        if (!map.get(tX, tY)) {
          enemyDistGood = true;
        }
      }

      std::cout << tX << ' ' << tY << std::endl;

      enemies.push_back(Enemy(tX * 32, tY * 32));
    }

    gold_manager.setSpawnChance(u.get("goldspawns")->getCurrent());
    currentGun.applyUpgrades(u);
  }

  void draw(GameAssets *assets, int frame, int f2, Upgrades &u) {
    BeginMode2D(cam);
    map.draw(assets->fetchTexture("tilemap"));
    p.draw(assets->fetchTexture("crab"), frame, &cam, map.size, freezeFrames,
           assets->fetchShader("whitemask"), u.get("goldmagnet")->getCurrent());
    currentGun.draw();

    for (Enemy &e : enemies) {
      e.draw(assets, frame);
    }

    for (EnemyDeath &ed : enemydeaths) {
      if (f2 == 0)
        ed.incrementFrame();
      ed.draw(assets);
    }

    for (Bullet &b : bullets) {
      b.draw();
    }

    for (HealthPotionEffect &hpe : hpeffects) {
      hpe.draw();
    }

    gold_manager.draw(*assets);

    EndMode2D();

    if (levelFinished) {
      finishedText.draw();
      finishedText2.draw();
    }
  }

  void update(SoundManager &s, GameState &state, Upgrades &u,
              bool screenShakeEnabled) {
    if (freezeFrames > 0) {
      freezeFrames--;
      if (screenShakeEnabled) {
        screenShake.x += GetRandomValue(-10, 10);
        screenShake.y += GetRandomValue(-10, 10);
        cam.offset.x = camoffsetbase.x + screenShake.x;
        cam.offset.y = camoffsetbase.y + screenShake.y;
      }
      return;
    } else if (screenShakeEnabled) {
      cam.offset = camoffsetbase;
      screenShake = Vector2Zero();
    }

    float dt = GetFrameTime();

    update_hpotions(s);
    update_player(s, dt);
    update_gold(s, u);
    update_hpeffects();
    std::vector<int> to_erase_enemies;
    update_bullets(to_erase_enemies, s);
    update_enemies(dt, s, to_erase_enemies);
    update_enemy_deaths();

    std::sort(to_erase_enemies.begin(), to_erase_enemies.end(),
              std::greater<int>());
    to_erase_enemies.erase(
        std::unique(to_erase_enemies.begin(), to_erase_enemies.end()),
        to_erase_enemies.end());

    for (int i : to_erase_enemies) {
      Enemy &e = enemies[i];
      gold_manager.trySpawnAtPos(e.getX() + e.getW() / 2.0f,
                                 e.getY() + e.getH() / 2.0f, s,
                                 u.get("goldmultiplier")->getCurrent());
      enemydeaths.push_back(EnemyDeath(e.getX(), e.getY()));
      enemies.erase(enemies.begin() + i);
    }

    if (p.health <= 0 && !dead) {
      dead = true;
      freezeFrames = 500;
      s.play("death");
    }

    if (dead && freezeFrames <= 0) {
      state = GAME_STATE_DEAD;
    }

    if (enemies.size() == 0 && !levelFinished && enemydeaths.size() == 0) {
      levelFinished = true;
      s.play("levelup");
    }

    if (levelFinished && IsKeyPressed(KEY_Q)) {
      state = GAME_STATE_SHOP;
    }
  }
};
