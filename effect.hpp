#pragma once
#include "raylib.h"
#include "raymath.h"
#include <algorithm>
#include <memory.h>
#include <vector>

class Particle {
protected:
  int x;
  int y;
  int r;
  Color clr;
  int lifetime;
  int lifetimeBase;

public:
  void draw() { DrawCircle(x, y, r, clr); }

  void update(float dt) {
    clr.a = 255 / (lifetimeBase / lifetime);
    lifetime--;
  }

  int getLifetime() { return lifetime; };
};

class HealthPotionParticle : public Particle {
public:
  HealthPotionParticle(int x, int y) : Particle() {
    this->x = x;
    this->y = y;
    r = 5;
    clr = GREEN;
    lifetime = 15;
    lifetimeBase = lifetime;
  };
};

class HealthPotionEffect {
private:
  std::vector<HealthPotionParticle> particles;
  int radius = 20;
  int minParticles = 5;
  int maxParticles = 10;
  int minRadiusRandomness = 0;
  int maxRadiusRandomness = 7;

public:
  HealthPotionEffect(int x, int y) {
    int pa = GetRandomValue(minParticles, maxParticles);
    for (int i = 0; i < pa; i++) {
      int r = radius + GetRandomValue(minRadiusRandomness, maxRadiusRandomness);

      float angle = 360.0f / pa * i;
      float angleRad = -angle * DEG2RAD;
      Vector2 sOffset = Vector2Scale({cosf(angleRad), -sinf(angleRad)}, -r);
      Vector2 origin = {(float)x, (float)y};
      Vector2 sPos = Vector2Add(origin, sOffset);
      int px = sPos.x;
      int py = sPos.y;
      particles.push_back(HealthPotionParticle(px, py));
    }
  }

  void draw() {
    for (HealthPotionParticle &hpp : particles) {
      hpp.draw();
    }
  }
  void update() {
    std::vector<int> to_remove;
    for (int i = 0; i < particles.size(); i++) {
      HealthPotionParticle &hpp = particles[i];
      hpp.update(GetFrameTime());
      if (hpp.getLifetime() <= 0)
        to_remove.push_back(i);
    }

    std::sort(to_remove.begin(), to_remove.end(), std::greater<int>());
    to_remove.erase(std::unique(to_remove.begin(), to_remove.end()),
                    to_remove.end());

    for (int i : to_remove)
      particles.erase(particles.begin() + i);
  }
};
