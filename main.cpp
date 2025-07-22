#include "game.hpp"
#include "raylib.h"
#include "utils.hpp"

int main() {
  SetTraceLogLevel(LOG_WARNING);

  InitWindow(600, 450, "Peanut Butter");

  SetTargetFPS(60);

  SetWindowState(FLAG_WINDOW_RESIZABLE);
  InitAudioDevice();

  RenderTexture2D target = LoadRenderTexture(320, 240);

  Game game;

  bool should_exit = false;

  while (!WindowShouldClose() && !should_exit) {
    ClearBackground(WHITE);
    game.update(&should_exit);

    BeginTextureMode(target);
    ClearBackground(BLACK);
    game.draw();

    EndTextureMode();

    BeginDrawing();
    DrawRenderTexture(target, 320, 240);
    DrawFPS(0, 0);
    EndDrawing();
  }

  UnloadRenderTexture(target);

  CloseWindow();
  return 0;
}
