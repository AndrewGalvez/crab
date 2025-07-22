#pragma once
#include "game_map.hpp"
#include "raylib.h"

class Player {
private:
  int speed = 100;
  int animY = 0;

public:
  int x;
  int y;
  int w;
  int h;
  int health = 100;

  Player(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {}

  void draw(Texture2D *tex, int frame, Camera2D *cam, int mapSize,
            int freezeFrames, Shader *wshader) {
    cam->target.x = this->x + (float)this->w / 2;
    cam->target.y = this->y + (float)this->h / 2;

    if (cam->target.x - cam->offset.x < 0)
      cam->target.x = cam->offset.x;
    if (cam->target.y - cam->offset.y < 0)
      cam->target.y = cam->offset.y;
    if (cam->target.x + cam->offset.x > mapSize)
      cam->target.x = mapSize - cam->offset.x;
    if (cam->target.y + cam->offset.y > mapSize)
      cam->target.y = mapSize - cam->offset.y;

    if (freezeFrames > 0)
      BeginShaderMode(*wshader);

    DrawTexturePro(*tex, {(float)16 * frame, (float)animY, 16, 16},
                   {(float)x, (float)y, (float)w, (float)h}, {0, 0}, 0,
                   RAYWHITE);
    if (freezeFrames > 0)
      EndShaderMode();
  };

  void update(float dt, int mapSize, GameMap &map) {
    int moveSpeed = this->speed * dt;
    int prevX = x;
    int prevY = y;
    this->animY = 0;

    // Handle input
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
      this->x -= moveSpeed;
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
      this->y -= moveSpeed;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
      this->x += moveSpeed;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
      this->y += moveSpeed;

    // Check horizontal collision (X-axis movement)
    if (this->x != prevX) {
      // Calculate all four corners for horizontal collision
      int leftTileX = this->x / map.TILE_SIZE;
      int rightTileX = (this->x + this->w - 1) / map.TILE_SIZE;
      int topTileY = this->y / map.TILE_SIZE;
      int bottomTileY = (this->y + this->h - 1) / map.TILE_SIZE;

      // Check if any corner hits a wall
      bool collision = false;

      // Check bounds first
      if (leftTileX < 0 || rightTileX >= map.tile_num || topTileY < 0 ||
          bottomTileY >= map.tile_num) {
        collision = true;
      } else {
        // Check all four corners for horizontal movement
        if (map.get(leftTileX, topTileY) ||     // Top-left
            map.get(rightTileX, topTileY) ||    // Top-right
            map.get(leftTileX, bottomTileY) ||  // Bottom-left
            map.get(rightTileX, bottomTileY)) { // Bottom-right
          collision = true;
        }
      }

      if (collision) {
        this->x = prevX;
      }
    }

    // Check vertical collision (Y-axis movement)
    if (this->y != prevY) {
      // Calculate all four corners for vertical collision
      int leftTileX = this->x / map.TILE_SIZE;
      int rightTileX = (this->x + this->w - 1) / map.TILE_SIZE;
      int topTileY = this->y / map.TILE_SIZE;
      int bottomTileY = (this->y + this->h - 1) / map.TILE_SIZE;

      // Check if any corner hits a wall
      bool collision = false;

      // Check bounds first
      if (leftTileX < 0 || rightTileX >= map.tile_num || topTileY < 0 ||
          bottomTileY >= map.tile_num) {
        collision = true;
      } else {
        // Check all four corners for vertical movement
        if (map.get(leftTileX, topTileY) ||     // Top-left
            map.get(rightTileX, topTileY) ||    // Top-right
            map.get(leftTileX, bottomTileY) ||  // Bottom-left
            map.get(rightTileX, bottomTileY)) { // Bottom-right
          collision = true;
        }
      }

      if (collision) {
        this->y = prevY;
      }
    }

    // Set animation state
    if (this->x != prevX || this->y != prevY) {
      animY = 16;
    }
  }

  void findSpawn(GameMap &map) {
    // Search for empty tile starting from top-left
    for (int row = 0; row < map.tile_num; row++) {
      for (int col = 0; col < map.tile_num; col++) {
        // Check if this tile is empty (false = walkable)
        if (!map.get(col, row)) {
          // Found an empty tile, set player position
          this->x = col * map.TILE_SIZE;
          this->y = row * map.TILE_SIZE;
          return;
        }
      }
    }

    // If no empty tile found, spawn at (0,0) as fallback
    this->x = 0;
    this->y = 0;
  }
};
