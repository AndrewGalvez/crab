#pragma once
#include "inventory.hpp"
#include <cmath>
#include <iostream>
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

  void setCurrent(float c) { current = c; }

  void setCost(int c) { cost = c; }
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
    inv.removeGold(cost);
    this->cost = floor((this->cost + 1) * 1.5);
  }
};

class Upgrade_Gold_Magnet : public Upgrade {
public:
  Upgrade_Gold_Magnet() : Upgrade() {
    current = 25.0f;
    cost = 1;
    id = "goldmagnet";
    name = "Gold Magnet";
    texid = "u_goldmagnet";
  }
  void buy(Inventory &inv) override {
    current *= 2;
    inv.removeGold(cost);
    cost += 2;
  }
};

class Upgrade_Bullet_Speed : public Upgrade {
public:
  Upgrade_Bullet_Speed() : Upgrade() {
    current = 5.0f;
    cost = 1;
    id = "bulletspeed";
    name = "Bullet Speed";
    texid = "u_bulletspeed";
  };

  void buy(Inventory &inv) override {
    current *= 1.5;
    inv.removeGold(cost);
    cost += 2;
  }
};

class Upgrade_Bullet_Ricochet : public Upgrade {
public:
  Upgrade_Bullet_Ricochet() : Upgrade() {
    current = 0.0f;
    cost = 5;
    id = "bulletricochet";
    name = "Ricochet";
    texid = "u_bulletricochet";
  };

  void buy(Inventory &inv) override {
    ++current;
    inv.removeGold(cost);
    cost *= 2;
  }
};

class Upgrades {
private:
  std::map<std::string, std::unique_ptr<Upgrade>> upgrades;

public:
  std::string getDataForSave() {
    std::cout << "=== getDataForSave() called ===" << std::endl;
    std::cout << "this pointer: " << this << std::endl;
    std::cout << "upgrades map address: " << &upgrades << std::endl;

    // Check if 'this' pointer looks valid
    if (this == nullptr) {
      std::cout << "ERROR: this pointer is null!" << std::endl;
      return "";
    }

    std::string data;

    // Try to get size safely
    size_t map_size = 0;
    try {
      map_size = upgrades.size();
      std::cout << "Map size: " << map_size << std::endl;
    } catch (...) {
      std::cout << "ERROR: Exception getting map size - map is corrupted!"
                << std::endl;
      return "";
    }

    // If size is absurdly large, the map is corrupted
    if (map_size > 1000) {
      std::cout << "ERROR: Map size is " << map_size
                << " - memory corruption detected!" << std::endl;
      std::cout << "Attempting to reinitialize..." << std::endl;

      try {
        upgrades.clear();
        init();
        map_size = upgrades.size();
        std::cout << "After reinit, map size: " << map_size << std::endl;
      } catch (...) {
        std::cout << "ERROR: Cannot recover from corruption" << std::endl;
        return "";
      }
    }

    // Ensure upgrades are initialized
    if (map_size == 0) {
      std::cout << "Upgrades map is empty, initializing...\n";
      init();
      map_size = upgrades.size();
      std::cout << "After init, map size: " << map_size << std::endl;
    }

    if (map_size != 5) {
      std::cout << "WARNING: Expected 5 upgrades, got " << map_size
                << std::endl;
    }

    try {
      std::cout << "About to iterate over map...\n";

      // Use a counter to detect infinite loops
      int count = 0;
      const int MAX_ITERATIONS = 10; // Safety limit

      for (auto it = upgrades.begin();
           it != upgrades.end() && count < MAX_ITERATIONS; ++it, ++count) {
        std::cout << "Iteration " << count << ": key = '" << it->first << "'"
                  << std::endl;

        if (!it->second) {
          std::cout << "Warning: null upgrade pointer for key: " << it->first
                    << std::endl;
          continue;
        }

        try {
          std::string id = it->second->getId();
          float current = it->second->getCurrent();
          int cost = it->second->getCost();

          data += id;
          data += ' ';
          data += std::to_string(current);
          data += ' ';
          data += std::to_string(cost);
          data += '\n';

          std::cout << "Successfully processed: " << id << std::endl;
        } catch (...) {
          std::cout << "ERROR: Exception processing upgrade " << it->first
                    << std::endl;
          continue;
        }
      }

      if (count >= MAX_ITERATIONS) {
        std::cout << "ERROR: Hit iteration limit - infinite loop detected!"
                  << std::endl;
      }

      std::cout << "Completed iteration, processed " << count << " upgrades\n";
      return data;

    } catch (const std::exception &e) {
      std::cout << "Exception in iterator: " << e.what() << std::endl;
      return "";
    } catch (...) {
      std::cout << "Unknown exception in iterator\n";
      return "";
    }
  }

  void loadDataFromSave(const std::string &data) {
    // Initialize if empty
    if (upgrades.empty()) {
      init();
    }

    size_t pos = 0;

    while (pos < data.length()) {
      size_t lineEnd = data.find('\n', pos);
      if (lineEnd == std::string::npos)
        lineEnd = data.length();

      std::string line = data.substr(pos, lineEnd - pos);
      pos = lineEnd + 1;

      if (line.empty())
        continue;

      size_t space1 = line.find(' ');
      size_t space2 = line.find(' ', space1 + 1);

      if (space1 != std::string::npos && space2 != std::string::npos) {
        std::string id = line.substr(0, space1);

        try {
          float current =
              std::stod(line.substr(space1 + 1, space2 - space1 - 1));
          int cost = std::stoi(line.substr(space2 + 1));

          auto it = upgrades.find(id);
          if (it != upgrades.end() && it->second) {
            it->second->setCurrent(current);
            it->second->setCost(cost);
          }
        } catch (const std::exception &e) {
          std::cout << "Error parsing line: " << line << " - " << e.what()
                    << std::endl;
          continue;
        }
      }
    }
  }

  void init() {
    std::cout << "Starting init()...\n";

    // Clear existing upgrades first
    upgrades.clear();
    std::cout << "Cleared existing upgrades\n";

    std::cout << "Creating goldmultiplier...\n";
    upgrades["goldmultiplier"] = std::make_unique<Upgrade_Gold_Multiplier>();
    std::cout << "Created goldmultiplier, map size: " << upgrades.size()
              << std::endl;

    std::cout << "Creating goldspawns...\n";
    upgrades["goldspawns"] = std::make_unique<Upgrade_Gold_Spawns>();
    std::cout << "Created goldspawns, map size: " << upgrades.size()
              << std::endl;

    std::cout << "Creating goldmagnet...\n";
    upgrades["goldmagnet"] = std::make_unique<Upgrade_Gold_Magnet>();
    std::cout << "Created goldmagnet, map size: " << upgrades.size()
              << std::endl;

    std::cout << "Creating bulletspeed...\n";
    upgrades["bulletspeed"] = std::make_unique<Upgrade_Bullet_Speed>();
    std::cout << "Created bulletspeed, map size: " << upgrades.size()
              << std::endl;

    std::cout << "Creating bulletricochet...\n";
    upgrades["bulletricochet"] = std::make_unique<Upgrade_Bullet_Ricochet>();
    std::cout << "Created bulletricochet, map size: " << upgrades.size()
              << std::endl;

    std::cout << "Init complete. Final map size: " << upgrades.size()
              << std::endl;

    // Verify all pointers are valid
    for (const auto &[key, upgrade] : upgrades) {
      if (!upgrade) {
        std::cout << "ERROR: Null pointer for " << key << std::endl;
      } else {
        std::cout << "Verified " << key << " - ID: " << upgrade->getId()
                  << std::endl;
      }
    }
  };

  Upgrades() { init(); }

  void buyUpgrade(const std::string &key, Inventory &inv) {
    auto it = upgrades.find(key);
    if (it != upgrades.end() && it->second) {
      it->second->buy(inv);
    }
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
