#pragma once
class Inventory {
private:
  int gold = 0;
  int hPotions = 0;

public:
  void addGold(int amount) { gold += amount; }
  void removeGold(int amount) { gold -= amount; }

  int getGold() { return gold; }

  void addHPotions(int amount) { hPotions += amount; }
  void removeHPotions(int amount) { hPotions -= amount; }
  int gethPotions() { return hPotions; }
};
