#pragma once
#include <algorithm>
#include <array>
#include <climits>
#include <cmath>
#include <cstdio>
#include <queue>
#include <vector>

struct Vec2 {
  int x, y;
  bool operator==(const Vec2 &o) const { return x == o.x && y == o.y; }
};

struct Node {
  Vec2 pos;
  int g, h;
  Vec2 parent;

  int f() const { return g + h; }
  bool operator>(const Node &o) const { return f() > o.f(); }
};

inline std::vector<Vec2> AStar(const std::array<std::array<bool, 20>, 20> &map,
                               Vec2 start, Vec2 end) {
  std::priority_queue<Node, std::vector<Node>, std::greater<Node>> open;
  std::array<std::array<bool, 20>, 20> closed{};
  std::array<std::array<Vec2, 20>, 20> parents{};
  std::array<std::array<int, 20>, 20> gScores{};

  // Initialize arrays
  for (int y = 0; y < 20; y++) {
    for (int x = 0; x < 20; x++) {
      parents[y][x] = {-1, -1};
      gScores[y][x] = INT_MAX;
      closed[y][x] = false;
    }
  }

  // Add start node
  int startH = std::abs(start.x - end.x) + std::abs(start.y - end.y);
  open.push({start, 0, startH, {-1, -1}});
  gScores[start.y][start.x] = 0;

  bool pathFound = false;
  int dirs[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

  int iterations = 0;
  while (!open.empty() && iterations < 1000) { // Safety limit
    iterations++;
    Node current = open.top();
    open.pop();

    // Skip if we've already processed this node with a better score
    if (closed[current.pos.y][current.pos.x])
      continue;

    // Mark as closed and set parent
    closed[current.pos.y][current.pos.x] = true;
    if (!(current.parent.x == -1 && current.parent.y == -1)) {
      parents[current.pos.y][current.pos.x] = current.parent;
    }

    // Check if we reached the goal
    if (current.pos == end) {
      pathFound = true;
      break;
    }

    // Explore neighbors
    for (auto &d : dirs) {
      Vec2 neighbor = {current.pos.x + d[0], current.pos.y + d[1]};

      // Check bounds
      if (neighbor.x < 0 || neighbor.y < 0 || neighbor.x >= 20 ||
          neighbor.y >= 20)
        continue;

      // Check if walkable (false = walkable, true = blocked)
      if (map[neighbor.y][neighbor.x])
        continue;

      // Skip if already closed
      if (closed[neighbor.y][neighbor.x])
        continue;

      int tentativeG = current.g + 1;

      // Skip if we've found a better path to this neighbor
      if (tentativeG >= gScores[neighbor.y][neighbor.x])
        continue;

      // This is a better path, record it
      gScores[neighbor.y][neighbor.x] = tentativeG;
      int h = std::abs(neighbor.x - end.x) + std::abs(neighbor.y - end.y);
      open.push({neighbor, tentativeG, h, current.pos});
    }
  }

  std::vector<Vec2> path;
  if (!pathFound) {
    return path;
  }

  // Reconstruct path
  Vec2 current = end;
  path.push_back(current);

  while (!(current == start)) {
    Vec2 parent = parents[current.y][current.x];

    // Safety check
    if (parent.x == -1 && parent.y == -1) {
      path.clear();
      return path;
    }

    path.push_back(parent);
    current = parent;
  }

  std::reverse(path.begin(), path.end());

  return path;
}
