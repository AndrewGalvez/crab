#pragma once
#include "inventory.hpp"
#include <cmath>
#include <map>
#include <memory>
#include <string>

class Upgrade {
protected:
  float current = 0;
  int cost = 0;
  std::string id;
  std::string name;
  std::string texid;

public:
  virtual void buy(Inventory &inv) {};

  float getCurrent() { return current; }

  int getCost() { return cost; }

  std::string getId() { return id; }
  std::string getTexID() { return texid; }
  std::string getName() { return name; }
};

class Upgrade_Gold_Multiplier : public Upgrade {
public:
  Upgrade_Gold_Multiplier() : Upgrade() {
    current = 1;
    cost = 5;
    id = "goldmultiplier";
    name = "Gold Multiplier";
    texid = "u_goldmultiplier";
  }

  void buy(Inventory &inv) override {
    this->current++;
    inv.removeGold(cost);
    this->cost = floor(this->cost * 1.5);
  }
};

class Upgrade_Gold_Spawns : public Upgrade {
public:
  Upgrade_Gold_Spawns() : Upgrade() {
    current = 0.5f;
    cost = 1;
    id = "goldspawns";
    name = "Gold Spawns";
    texid = "u_goldspawns";
  }

  void buy(Inventory &inv) override {
    current += (1.0f - this->current) / 10;
    inv.removeGold(inv.getGold());
    this->cost = floor((this->cost + 1) * 1.5);
  }
};

class Upgrades {
private:
  std::map<std::string, std::unique_ptr<Upgrade>> upgrades;

public:
  Upgrades() {
    upgrades["goldmultiplier"] = std::make_unique<Upgrade_Gold_Multiplier>();
    upgrades["goldspawns"] = std::make_unique<Upgrade_Gold_Spawns>();
  };

  void buyUpgrade(std::string key, Inventory &inv) {
    if (upgrades.find(key) != upgrades.end())
      upgrades.at(key)->buy(inv);
  }

  Upgrade *get(const std::string &k) const {
    auto it = upgrades.find(k);
    return (it != upgrades.end()) ? it->second.get() : nullptr;
  }

  std::string getNext(const std::string &currentKey) const {
    if (upgrades.empty()) {
      return ""; // Handle empty map case
    }

    auto it = upgrades.find(currentKey);
    if (it != upgrades.end()) {
      it++;
      if (it != upgrades.end()) {
        return it->first;
      } else {
        // Wrap around to beginning
        return upgrades.begin()->first;
      }
    } else {
      // Key not found, return first element
      return upgrades.begin()->first;
    }
  }
  std::string getPrev(const std::string &currentKey) const {
    if (upgrades.empty()) {
      return ""; // Handle empty map case
    }

    auto it = upgrades.find(currentKey);
    if (it != upgrades.end()) {
      if (it == upgrades.begin()) {
        // Wrap around to end
        return std::prev(upgrades.end())->first;
      } else {
        // Move to previous element
        it--;
        return it->first;
      }
    } else {
      // Key not found, return first element
      return upgrades.begin()->first;
    }
  }
};
