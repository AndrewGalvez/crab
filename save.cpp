#include "save.hpp"
#include "game.hpp"
#include <fstream>
#include <iostream>

void SaveManager::save_game() {
  std::string data;

  std::string data_runner;
  {
    std::string data_runner_player_health = std::to_string(g->runner.p.health);
    std::string data_runner_player_x = std::to_string(g->runner.p.x);
    std::string data_runner_player_y = std::to_string(g->runner.p.y);

    data_runner += data_runner_player_health;
    data_runner += ' ';
    data_runner += data_runner_player_x;
    data_runner += ' ';
    data_runner += data_runner_player_y;
    data_runner += '\n';
  }

  std::string data_inventory;
  {
    std::string data_inventory_gold = std::to_string(g->inventory.getGold());
    std::string data_inventory_hpotions =
        std::to_string(g->inventory.gethPotions());
    data_inventory += data_inventory_gold;
    data_inventory += ' ';
    data_inventory += data_inventory_hpotions;
  }
  std::string data_upgrades = g->upgrades.getDataForSave();
  data += data_inventory;
  data += '\n';
  data += data_runner;
  data += data_upgrades;
  std::cout << "made data\n";

  std::ofstream stream("data/gamesave");
  stream << data;
  stream.close();
}

void SaveManager::load_game() {
  std::ifstream stream("data/gamesave");
  if (!stream)
    return;

  std::string line;

  // Read inventory line
  if (std::getline(stream, line)) {
    size_t space_pos = line.find(' ');
    if (space_pos != std::string::npos) {
      std::string gold_str = line.substr(0, space_pos);
      std::string hpotions_str = line.substr(space_pos + 1);
      int gold = std::stoi(gold_str);
      int hpotions = std::stoi(hpotions_str);
      g->inventory.setGold(gold);
      g->inventory.sethPotions(hpotions);
    }
  }
  // Read runner line
  if (std::getline(stream, line)) {
    size_t space_pos = line.find(' ');
    if (space_pos != std::string::npos) {
      std::string health_str = line.substr(0, space_pos);
      std::string n_str = line.substr(space_pos + 1);
      int health = std::stoi(health_str);
      g->runner.p.health = health;

      size_t space_pos_2 = n_str.find(' ');
      std::string x_str = n_str.substr(0, space_pos_2);
      std::string y_str = n_str.substr(space_pos_2 + 1);

      int x = std::stoi(x_str);
      int y = std::stoi(y_str);

      g->runner.p.x = x;
      g->runner.p.y = y;
    }
  }

  // Read ALL remaining lines for upgrades
  std::string upgrades_data;
  while (std::getline(stream, line)) {
    upgrades_data += line + '\n';
  }

  if (!upgrades_data.empty()) {
    g->upgrades.loadDataFromSave(upgrades_data);
  }

  stream.close();
}
