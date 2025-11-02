#pragma once
#include "raylib.h"
#include <cmath>
#include <raymath.h>

class GUIText {
private:
  int x;
  int y;
  int fs;
  Vector2 offset = Vector2Zero();
  const char *value;
  Color clr;
  bool draw_centered;

public:
  GUIText(int x, int y, int font_size, const char *value, Color color,
          bool draw_centered)
      : x(x), y(y), fs(font_size), value(value), clr(color),
        draw_centered(draw_centered) {};

  void draw() {
    int nx = draw_centered ? x - MeasureText(value, fs) / 2 : x;
    DrawText(value, nx + offset.x, y + offset.y, fs, clr);
  }

  void setPos(int x, int y) {
    this->x = x;
    this->y = y;
  }

  void setFontSize(int font_size) { this->fs = font_size; }

  void setColor(Color color) { this->clr = color; }

  void setDrawCentered(bool x) { draw_centered = x; }

  void setValue(const char *n) { value = n; }

  void setOffset(Vector2 n) { offset = n; }
};

class GUIButton {
private:
  int x;
  int y;
  int w;
  int h;
  Color clr;
  Color clr_hover;
  bool draw_centered;
  GUIText text;
  bool playSoundOnClick = false;
  bool enabled = true;

public:
  GUIButton(int x, int y, int w, int h, Color color, Color color_hover,
            bool draw_centered, GUIText text, bool playSound = false,
            bool enabled = true)
      : x(x), y(y), w(w), h(h), clr(color), clr_hover(color_hover),
        draw_centered(draw_centered), text(text), playSoundOnClick(playSound),
        enabled(enabled) {};

  bool isMouseOn() {
    int targetWidth = 320;
    int targetHeight = 240;
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    float scale = fminf((float)screenWidth / targetWidth,
                        (float)screenHeight / targetHeight);
    int renderWidth = (int)(targetWidth * scale);
    int renderHeight = (int)(targetHeight * scale);
    int offsetX = (screenWidth - renderWidth) / 2;
    int offsetY = (screenHeight - renderHeight) / 2;

    Vector2 mp = GetMousePosition();
    mp.x = (mp.x - offsetX) * ((float)targetWidth / renderWidth);
    mp.y = (mp.y - offsetY) * ((float)targetHeight / renderHeight);

    return mp.x > x && mp.x < x + w && mp.y > y && mp.y < y + h;
  }

  void draw() {
    int nx = draw_centered ? x - w / 2 : x;
    Color nclr = !isMouseOn() && enabled ? clr : clr_hover;
    DrawRectangle(nx, y, w, h, nclr);
    text.draw();
  };

  void setPos(int x, int y) {
    this->x = x;
    this->y = y;
  }

  void setSize(int w, int h) {
    this->w = w;
    this->h = h;
  }

  void setColor(Color color) { this->clr = color; }
  void setHoverColor(Color color) { this->clr_hover = color; }

  void setDrawCentered(bool x) { draw_centered = x; }
};

class GUICheckbox {
private:
  int x;
  int y;
  int w;
  int h;

public:
  bool checked;
  GUICheckbox(int x, int y, int w, int h, bool checked)
      : x(x), y(y), w(w), h(h), checked(checked) {}

  void tick() { checked = !checked; }

  void draw() {
    DrawRectangle(x, y, w, h, WHITE);
    if (checked) {
      DrawRectangle(x + w / 4, y + h / 4, w / 2, h / 2, BLACK);
    }
  }

  bool isMouseOn() {
    int targetWidth = 320;
    int targetHeight = 240;
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    float scale = fminf((float)screenWidth / targetWidth,
                        (float)screenHeight / targetHeight);
    int renderWidth = (int)(targetWidth * scale);
    int renderHeight = (int)(targetHeight * scale);
    int offsetX = (screenWidth - renderWidth) / 2;
    int offsetY = (screenHeight - renderHeight) / 2;

    Vector2 mp = GetMousePosition();
    mp.x = (mp.x - offsetX) * ((float)targetWidth / renderWidth);
    mp.y = (mp.y - offsetY) * ((float)targetHeight / renderHeight);

    return mp.x > x && mp.x < x + w && mp.y > y && mp.y < y + h;
  }
};
