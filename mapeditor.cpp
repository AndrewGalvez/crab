#include "raylib.h"
#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 600;
const int MAP_SIZE = 20;
const float TILE_SIZE = 30.0f; // 600/20 = 30 pixels per tile

class TileMapEditor {
private:
  std::array<std::array<bool, MAP_SIZE>, MAP_SIZE> tilemap;
  bool isDrawing = false;
  bool currentTool = true; // true = wall (1), false = empty (0)
  bool showGrid = true;
  bool showFileDialog = false;
  bool saveMode = false; // true = save, false = load
  std::string currentFilename = "level1.txt";
  std::string fileInput = "";

  // Predefined filenames for quick access
  std::vector<std::string> quickFiles = {
      "level1.txt", "level2.txt", "level3.txt", "level4.txt", "level5.txt"};

public:
  TileMapEditor() {
    // Initialize tilemap with all false (empty)
    for (int i = 0; i < MAP_SIZE; i++) {
      for (int j = 0; j < MAP_SIZE; j++) {
        tilemap[i][j] = false;
      }
    }
  }

  void handleInput() {
    Vector2 mousePos = GetMousePosition();

    // Convert mouse position to tile coordinates
    int tileX = (int)(mousePos.x / TILE_SIZE);
    int tileY = (int)(mousePos.y / TILE_SIZE);

    // Ensure tile coordinates are within bounds
    if (tileX >= 0 && tileX < MAP_SIZE && tileY >= 0 && tileY < MAP_SIZE) {
      // Handle mouse drawing
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        isDrawing = true;
        tilemap[tileY][tileX] = currentTool;
      } else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && isDrawing) {
        tilemap[tileY][tileX] = currentTool;
      } else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        isDrawing = false;
      }

      // Right click for erasing
      if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        tilemap[tileY][tileX] = false;
      }
    }

    // Keyboard shortcuts
    if (IsKeyPressed(KEY_SPACE)) {
      currentTool = !currentTool; // Toggle tool
    }

    if (IsKeyPressed(KEY_C)) {
      clearMap();
    }

    if (IsKeyPressed(KEY_G)) {
      showGrid = !showGrid;
    }

    if (IsKeyPressed(KEY_S)) {
      showFileDialog = true;
      saveMode = true;
    }

    if (IsKeyPressed(KEY_L)) {
      showFileDialog = true;
      saveMode = false;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
      showFileDialog = false;
      fileInput = "";
    }

    // Handle file dialog input
    if (showFileDialog) {
      handleFileDialogInput();
    }

    // Quick save/load with number keys
    for (int i = 0; i < 5; i++) {
      if (IsKeyPressed(KEY_ONE + i)) {
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
          // Shift + number = save
          saveMap(quickFiles[i].c_str());
        } else {
          // Number = load
          loadMap(quickFiles[i].c_str());
        }
      }
    }
  }

  void render() {
    BeginDrawing();
    ClearBackground(DARKGRAY);

    // Draw tiles
    for (int y = 0; y < MAP_SIZE; y++) {
      for (int x = 0; x < MAP_SIZE; x++) {
        Rectangle tileRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE,
                              TILE_SIZE};

        // Draw tile based on value
        if (tilemap[y][x]) {
          DrawRectangleRec(tileRect, WHITE);
        } else {
          DrawRectangleRec(tileRect, BLACK);
        }

        // Draw grid lines
        if (showGrid) {
          DrawRectangleLinesEx(tileRect, 1, GRAY);
        }
      }
    }

    // Draw UI
    drawUI();

    // Draw file dialog if active
    if (showFileDialog) {
      drawFileDialog();
    }

    EndDrawing();
  }

  void drawUI() {
    // Tool indicator
    const char *toolText = currentTool ? "Tool: WALL (1)" : "Tool: EMPTY (0)";
    DrawText(toolText, 10, 10, 20, currentTool ? WHITE : BLACK);

    // Instructions
    DrawText("Controls:", 10, 40, 16, YELLOW);
    DrawText("Left Click: Draw", 10, 60, 12, WHITE);
    DrawText("Right Click: Erase", 10, 75, 12, WHITE);
    DrawText("SPACE: Toggle Tool", 10, 90, 12, WHITE);
    DrawText("C: Clear Map", 10, 105, 12, WHITE);
    DrawText("G: Toggle Grid", 10, 120, 12, WHITE);
    DrawText("S: Save Map", 10, 135, 12, WHITE);
    DrawText("L: Load Map", 10, 150, 12, WHITE);
    DrawText("ESC: Cancel Dialog", 10, 165, 12, WHITE);

    // Quick save/load
    DrawText("Quick Save/Load:", 10, 185, 14, YELLOW);
    DrawText("1-5: Load level", 10, 205, 12, WHITE);
    DrawText("Shift+1-5: Save level", 10, 220, 12, WHITE);

    // Grid toggle indicator
    if (showGrid) {
      DrawText("Grid: ON", 10, 240, 12, GREEN);
    } else {
      DrawText("Grid: OFF", 10, 240, 12, RED);
    }

    // Current file
    DrawText(TextFormat("File: %s", currentFilename.c_str()), 10, 260, 12,
             LIGHTGRAY);

    // Mouse position info
    Vector2 mousePos = GetMousePosition();
    int tileX = (int)(mousePos.x / TILE_SIZE);
    int tileY = (int)(mousePos.y / TILE_SIZE);

    if (tileX >= 0 && tileX < MAP_SIZE && tileY >= 0 && tileY < MAP_SIZE) {
      DrawText(TextFormat("Tile: %d,%d", tileX, tileY), 10, 280, 12, LIGHTGRAY);
    }
  }

  void handleFileDialogInput() {
    // Handle text input
    int key = GetCharPressed();
    while (key > 0) {
      if (key >= 32 && key <= 125 && fileInput.length() < 50) {
        fileInput += (char)key;
      }
      key = GetCharPressed();
    }

    // Handle backspace
    if (IsKeyPressed(KEY_BACKSPACE) && fileInput.length() > 0) {
      fileInput.pop_back();
    }

    // Handle enter
    if (IsKeyPressed(KEY_ENTER) && !fileInput.empty()) {
      currentFilename = fileInput;
      if (saveMode) {
        saveMap(currentFilename.c_str());
      } else {
        loadMap(currentFilename.c_str());
      }
      showFileDialog = false;
      fileInput = "";
    }
  }

  void drawFileDialog() {
    // Draw overlay
    DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Fade(BLACK, 0.7f));

    // Draw dialog box
    Rectangle dialogBox = {150, 200, 300, 200};
    DrawRectangleRec(dialogBox, DARKGRAY);
    DrawRectangleLinesEx(dialogBox, 2, WHITE);

    // Title
    const char *title = saveMode ? "Save Map" : "Load Map";
    DrawText(title, 200, 220, 20, WHITE);

    // Quick options
    DrawText("Quick options:", 170, 250, 14, YELLOW);
    for (int i = 0; i < 5; i++) {
      Color color = WHITE;
      DrawText(TextFormat("%d: %s", i + 1, quickFiles[i].c_str()), 170,
               270 + i * 15, 12, color);
    }

    // Input field
    DrawText("Or type filename:", 170, 355, 12, WHITE);
    Rectangle inputBox = {170, 370, 260, 20};
    DrawRectangleRec(inputBox, WHITE);
    DrawRectangleLinesEx(inputBox, 1, BLACK);
    DrawText(fileInput.c_str(), 175, 375, 12, BLACK);

    // Instructions
    DrawText("ENTER: Confirm | ESC: Cancel", 170, 395, 10, LIGHTGRAY);

    // Handle quick selection
    for (int i = 0; i < 5; i++) {
      if (IsKeyPressed(KEY_ONE + i)) {
        currentFilename = quickFiles[i];
        if (saveMode) {
          saveMap(currentFilename.c_str());
        } else {
          loadMap(currentFilename.c_str());
        }
        showFileDialog = false;
        fileInput = "";
        break;
      }
    }
  }

  void clearMap() {
    for (int i = 0; i < MAP_SIZE; i++) {
      for (int j = 0; j < MAP_SIZE; j++) {
        tilemap[i][j] = false;
      }
    }
  }

  void saveMap(const char *filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
      std::cout << "Error: Could not save file " << filename << std::endl;
      return;
    }

    for (int y = 0; y < MAP_SIZE; y++) {
      for (int x = 0; x < MAP_SIZE; x++) {
        file << (tilemap[y][x] ? '1' : '0');
      }
      file << std::endl;
    }

    file.close();
    currentFilename = filename;
    std::cout << "Map saved to " << filename << std::endl;
  }

  void loadMap(const char *filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
      std::cout << "Error: Could not load file " << filename << std::endl;
      return;
    }

    std::string line;
    int y = 0;

    while (std::getline(file, line) && y < MAP_SIZE) {
      for (int x = 0; x < line.length() && x < MAP_SIZE; x++) {
        tilemap[y][x] = (line[x] == '1');
      }
      y++;
    }

    file.close();
    currentFilename = filename;
    std::cout << "Map loaded from " << filename << std::endl;
  }

  void run() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Tile Map Editor - 20x20");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
      handleInput();
      render();
    }

    CloseWindow();
  }
};

#if BUILD_EDITOR == 0
int main() {
  TileMapEditor editor;
  editor.run();
  return 0;
}
#endif
// Compilation command (make sure you have raylib installed):
// g++ -o map_editor map_editor.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
