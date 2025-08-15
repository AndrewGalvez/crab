#pragma once

class Game;

class SaveManager {
public:
  void save_game();
  void load_game();
  Game *g;
};
