#pragma once
class Inventory {
private:
  int gold = 0;
  int hPotions = 2; // starting amount

public:
  void reset() {
    gold = 0;
    hPotions = 2;
  }

  void setGold(int amount) { gold = amount; }
  void addGold(int amount) { gold += amount; }
  void removeGold(int amount) { gold -= amount; }

  int getGold() { return gold; }

  void sethPotions(int amount) { hPotions = amount; }
  void addHPotions(int amount) { hPotions += amount; }
  void removeHPotions(int amount) { hPotions -= amount; }
  int gethPotions() { return hPotions; }
};
