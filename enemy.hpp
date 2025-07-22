#pragma once
#include "game_assets.hpp"
#include "pathfinding.hpp"
#include "player.hpp"
#include "raylib.h"
#include <vector>

class Enemy {
private:
  float x;
  float y;
  int w = 32;
  int h = 32;
  int speed = 75;
  std::vector<Vec2> path;
  int pathIndex = 0;
  int searchPrecision = 1; // Slightly larger tolerance
  float pathfindTimer = 0.0f;
  float pathfindInterval = 15.0f; // Pathfind every 15 frames
                                  //
  int animY = 0;

public:
  Enemy(int startX, int startY) : x((float)startX), y((float)startY) {}

  void draw(GameAssets *assets, int frame) {
    DrawTexturePro(*assets->fetchTexture("enemy"),
                   {(float)16 * (frame % 4), (float)animY, 16, 16},
                   (Rectangle){x, y, (float)w, (float)h}, {0, 0}, 0.0f, WHITE);
  }

  Vec2
  findNearestWalkableTile(Vec2 pos,
                          const std::array<std::array<bool, 20>, 20> &map) {
    // First check if the position itself is walkable
    if (pos.x >= 0 && pos.x < 20 && pos.y >= 0 && pos.y < 20 &&
        !map[pos.y][pos.x]) {
      return pos;
    }

    // Search in expanding squares around the position
    for (int radius = 1; radius <= 5; radius++) {
      for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
          // Only check the perimeter of the current radius
          if (abs(dx) != radius && abs(dy) != radius)
            continue;

          int nx = pos.x + dx;
          int ny = pos.y + dy;

          if (nx >= 0 && nx < 20 && ny >= 0 && ny < 20 && !map[ny][nx]) {
            return {nx, ny};
          }
        }
      }
    }

    // If no walkable tile found, return original position
    return pos;
  }

  // Check if the enemy can move to a specific position
  bool canMoveTo(float newX, float newY,
                 const std::array<std::array<bool, 20>, 20> &map) {
    // Get the tile coordinates for all four corners of the enemy
    int leftTile = (int)(newX / 32);
    int rightTile = (int)((newX + w - 1) / 32);
    int topTile = (int)(newY / 32);
    int bottomTile = (int)((newY + h - 1) / 32);

    // Clamp to map bounds
    leftTile = std::max(0, std::min(19, leftTile));
    rightTile = std::max(0, std::min(19, rightTile));
    topTile = std::max(0, std::min(19, topTile));
    bottomTile = std::max(0, std::min(19, bottomTile));

    // Check all tiles that the enemy would occupy
    for (int tileY = topTile; tileY <= bottomTile; tileY++) {
      for (int tileX = leftTile; tileX <= rightTile; tileX++) {
        if (map[tileY][tileX]) { // true = blocked
          return false;
        }
      }
    }
    return true;
  }

  void update(float dt, Player &p, std::array<std::array<bool, 20>, 20> &map) {
    // Update pathfinding timer
    pathfindTimer++;
    if (pathfindTimer >= pathfindInterval) {
      pathfind(p, map);
      pathfindTimer = 0.0f;
    }

    // Make sure we have a valid path
    if (path.empty() || pathIndex >= path.size()) {
      return;
    }

    // Current enemy center
    float enemyCenterX = x + w / 2.0f;
    float enemyCenterY = y + h / 2.0f;

    // Check if we should advance to the next waypoint with more generous
    // distance
    while (pathIndex < path.size()) {
      Vec2 currentTarget = path[pathIndex];
      float dx = currentTarget.x - enemyCenterX;
      float dy = currentTarget.y - enemyCenterY;
      float distance = sqrt(dx * dx + dy * dy);

      // More generous waypoint advancement - use larger radius
      if (distance <= 20.0f) { // Increased from 16.0f
        pathIndex++;
      } else {
        break;
      }
    }

    // If we've reached the end of the path, trigger new pathfinding
    if (pathIndex >= path.size()) {
      pathfindTimer = pathfindInterval;
      return;
    }

    // Move towards current target
    Vec2 dest = path[pathIndex];
    float dx = dest.x - enemyCenterX;
    float dy = dest.y - enemyCenterY;
    float distance = sqrt(dx * dx + dy * dy);

    if (distance > 0) {
      // Normalize movement direction
      float moveX = (dx / distance) * speed * dt;
      float moveY = (dy / distance) * speed * dt;

      // Calculate new positions
      float newX = x + moveX;
      float newY = y + moveY;

      // Set animation direction
      animY = moveX > 0 ? 0 : 16;

      // Try different movement strategies
      bool moved = false;

      // 1. Try full diagonal movement
      if (canMoveTo(newX, newY, map)) {
        x = newX;
        y = newY;
        moved = true;
      }
      // 2. Try horizontal movement only
      else if (canMoveTo(newX, y, map)) {
        x = newX;
        moved = true;
      }
      // 3. Try vertical movement only
      else if (canMoveTo(x, newY, map)) {
        y = newY;
        moved = true;
      }
      // 4. Try smaller movements to "slide" along walls
      else {
        // Try half movement
        float halfMoveX = moveX * 0.5f;
        float halfMoveY = moveY * 0.5f;

        if (canMoveTo(x + halfMoveX, y + halfMoveY, map)) {
          x += halfMoveX;
          y += halfMoveY;
          moved = true;
        } else if (canMoveTo(x + halfMoveX, y, map)) {
          x += halfMoveX;
          moved = true;
        } else if (canMoveTo(x, y + halfMoveY, map)) {
          y += halfMoveY;
          moved = true;
        }
      }

      // If still stuck, try to find alternate path or skip current waypoint
      if (!moved && pathIndex < path.size() - 1) {
        // Skip to next waypoint if we can't reach current one
        Vec2 nextTarget = path[pathIndex + 1];
        float nextDx = nextTarget.x - enemyCenterX;
        float nextDy = nextTarget.y - enemyCenterY;
        float nextDistance = sqrt(nextDx * nextDx + nextDy * nextDy);

        // If next waypoint is reachable and not too far, skip current one
        if (nextDistance < 50.0f) { // Reasonable skip distance
          float nextMoveX = (nextDx / nextDistance) * speed * dt;
          float nextMoveY = (nextDy / nextDistance) * speed * dt;

          if (canMoveTo(x + nextMoveX, y + nextMoveY, map)) {
            pathIndex++; // Skip problematic waypoint
          }
        }
      }

      // Last resort: if completely stuck, request new pathfinding
      static int stuckCounter = 0;
      if (!moved) {
        stuckCounter++;
        if (stuckCounter > 30) {            // Stuck for ~30 frames
          pathfindTimer = pathfindInterval; // Force new pathfinding
          stuckCounter = 0;
        }
      } else {
        stuckCounter = 0;
      }
    }
  }

  // Also improve the pathfinding function to avoid tile centers
  void pathfind(Player &p, std::array<std::array<bool, 20>, 20> &map) {
    // Convert pixel coordinates to grid coordinates
    Vec2 start = {(int)((x + w / 2.0f) / 32), (int)((y + h / 2.0f) / 32)};
    Vec2 end = {(int)(p.x / 32), (int)(p.y / 32)};

    // Clamp to map bounds
    start.x = std::max(0, std::min(19, start.x));
    start.y = std::max(0, std::min(19, start.y));
    end.x = std::max(0, std::min(19, end.x));
    end.y = std::max(0, std::min(19, end.y));

    // Find nearest walkable tiles if needed
    Vec2 walkableStart = findNearestWalkableTile(start, map);
    Vec2 walkableEnd = findNearestWalkableTile(end, map);

    // Get path in grid coordinates
    std::vector<Vec2> gridPath = AStar(map, walkableStart, walkableEnd);

    if (gridPath.empty()) {
      return;
    }

    // Convert back to pixel coordinates with better positioning
    path.clear();
    for (size_t i = 0; i < gridPath.size(); i++) {
      const Vec2 &gridPos = gridPath[i];

      // Instead of always using tile center, adjust based on movement direction
      Vec2 pixelPos;

      if (i == gridPath.size() - 1) {
        // Last waypoint: aim for player's actual position
        pixelPos = {static_cast<int>((int)p.x + p.w / 2.0f),
                    static_cast<int>((p.y + p.h / 2.0f))};
      } else {
        // Intermediate waypoints: use tile centers but add some offset for
        // smoother movement
        pixelPos = {gridPos.x * 32 + 16, gridPos.y * 32 + 16};

        // Add small random offset to avoid always hitting exact same spots
        pixelPos.x += (rand() % 8 - 4); // Random offset ±4 pixels
        pixelPos.y += (rand() % 8 - 4);
      }

      path.push_back(pixelPos);
    }

    pathIndex =
        path.size() > 0 ? 1 : 0; // Start from index 1 (skip current position)
  }
  // Getter methods
  float getX() const { return x; }
  float getY() const { return y; }
  int getW() const { return w; }
  int getH() const { return h; }

  // Debug methods
  bool hasPath() const { return !path.empty() && pathIndex < path.size(); }
  int getPathIndex() const { return pathIndex; }
  int getPathSize() const { return path.size(); }
};
