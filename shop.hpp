#pragma once
#include "game_assets.hpp"
#include "game_runner.hpp"
#include "game_state.hpp"
#include "gui.hpp"
#include "gun.hpp"
#include "inventory.hpp"
#include "sound.hpp"
#include "upgrades.hpp"
#include <memory.h>
#include <raylib.h>
#include <string>

struct CurrentUpgrade {
public:
  std::string index;
  std::string texid;

  GUIText name_text = GUIText(240, 130, 18, "Gold Multiplier", WHITE, true);
  GUIText cost_text = GUIText(240, 150, 18, "Cost: 0", WHITE, true);
  GUIText current_text = GUIText(240, 170, 18, "Current: 1.00", WHITE, true);
};

enum ShopState { SHOP_STATE_UPGRADES, SHOP_STATE_GUNS };

class ShopMenuUpgrades {
public:
  GUIButton button_left = GUIButton(
      240 - 32 - 30, 60 + 32 - 12, 24, 24, GRAY, DARKGRAY, false,
      GUIText(240 - 32 - 30 + 12, 60 + 33 - 12, 24, "([", BLACK, true), false);

  GUIButton button_right = GUIButton(
      240 + 32 + 6, 60 + 32 - 12, 24, 24, GRAY, DARKGRAY, false,
      GUIText(240 + 32 + 6 + 12, 60 + 33 - 12, 24, "])", BLACK, true), false);

  GUIButton button_buy = GUIButton(240 - 32, 200, 64, 32, GRAY, DARKGRAY, false,
                                   GUIText(240, 200, 30, "Buy", BLACK, true));
  CurrentUpgrade current_upgrade;

  void draw(GameAssets &assets) {
    current_upgrade.name_text.draw();
    current_upgrade.cost_text.draw();
    current_upgrade.current_text.draw();
    button_left.draw();
    button_right.draw();
    Texture2D *tex = assets.fetchTexture(current_upgrade.texid.c_str());

    DrawTexturePro(*tex, {0, 0, 16, 16}, {240 - 32, 60, 64, 64}, {0, 0}, 0.0f,
                   WHITE);

    grain(assets);

    button_buy.draw();

    EndShaderMode();
  }

  void update(Upgrades &upgrades, Inventory &inv, SoundManager &s_manager) {
    Upgrade *u = upgrades.get(current_upgrade.index);

    if (!u)
      return;

    static std::string untxt;
    untxt = u->getName();
    current_upgrade.name_text.setValue(untxt.c_str());

    static std::string uctxt;
    uctxt = "Cost: " + std::to_string(u->getCost());
    current_upgrade.cost_text.setValue(uctxt.c_str());

    static std::string ucrtxt;
    ucrtxt = "Current: " + std::to_string(u->getCurrent());
    ucrtxt.pop_back();
    ucrtxt.pop_back();
    ucrtxt.pop_back();
    current_upgrade.current_text.setValue(ucrtxt.c_str());

    button_buy.setColor(u->getCost() <= inv.getGold() ? GREEN : RED);
    button_buy.setHoverColor(u->getCost() <= inv.getGold() ? DARKGREEN : RED);
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      if (button_left.isMouseOn() || button_right.isMouseOn()) {
        s_manager.play("select");

        Upgrade *un = nullptr;

        if (button_left.isMouseOn())
          un = upgrades.get(upgrades.getPrev(current_upgrade.index));
        else
          un = upgrades.get(upgrades.getNext(current_upgrade.index));

        if (!un) {
          return;
        }

        current_upgrade.index = un->getId();
        current_upgrade.texid = un->getTexID();
        current_upgrade.name_text.setValue(un->getName().c_str());
      }

      if (button_buy.isMouseOn() && u->getCost() <= inv.getGold()) {
        s_manager.play("select");
        u->buy(inv);
      }
    }
  }
};

class GunDisplay {
  int x, y, w, h;
  Gun &g;
  GUIText nametxt;
  GUIText costtxt;
  GUIText ownedtxt;
  GUIButton buybutton;
  GUIButton usebutton;
  bool equipped = false;

public:
  GunDisplay(int x, int y, int w, int h, Gun &g)
      : x(x), y(y), w(w), h(h), g(g),
        nametxt(x, y, 16, g.displayName.c_str(), WHITE, false),
        buybutton(x + w * 1.5, y, w / 2, 20, GRAY, DARKGRAY, false,
                  GUIText(x + w * 1.5 + 1, y, 16, "Buy", BLACK, false), false),
        usebutton(x + w * 1.5, y, w / 2, 20, GRAY, DARKGRAY, false,
                  GUIText(x + w * 1.5 + 1, y, 16, "Use", BLACK, false), false),

        costtxt(x + w / 4, y + 20, 16,
                strdup(std::string("Cost: " + std::to_string(g.cost)).c_str()),
                WHITE, false),
        ownedtxt(x + w / 4, y + 40, 16, g.owned ? "Owned" : "Not Owned",
                 g.owned ? GREEN : RED, false) {}

  void draw(GameAssets &assets) {
    nametxt.draw();
    costtxt.draw();
    ownedtxt.draw();
    grain(assets);
    if (!g.owned)
      buybutton.draw();
    else if (!equipped)
      usebutton.draw();
    EndShaderMode();
  }

  void update(GameRunner *r, SoundManager &s) {
    buybutton.setColor(r->inv.getGold() >= g.cost ? GRAY : RED);
    buybutton.setHoverColor(r->inv.getGold() >= g.cost ? DARKGRAY : RED);
    equipped = r->currentGun.displayName == g.displayName;
    nametxt.setColor(equipped ? GOLD : WHITE);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      if (!g.owned) {
        if (buybutton.isMouseOn()) {
          if (r->inv.getGold() >= g.cost) {
            r->currentGun = g;
            r->inv.removeGold(g.cost);
            g.owned = true;
            ownedtxt.setColor(GREEN);
            ownedtxt.setValue("Owned");
          }
          s.play("select");
        }
      } else if (!equipped) {
        if (usebutton.isMouseOn()) {
          r->currentGun = g;
          s.play("select");
        }
      }
    }
  }
};

class ShopMenuGuns {
public:
  ShopMenuGuns() : ShopMenuGuns(nullptr) {};
  std::vector<GunDisplay> gundisplays = {};
  ShopMenuGuns(GameRunner *r) {
    if (r == nullptr)
      return;
    int y = 50;
    for (Gun &g : r->all_guns) {
      gundisplays.emplace_back(GunDisplay(180, y, 60, 24, g));
      y += 60;
    }
  }

  void draw(GameRunner *r, GameAssets &assets) {
    for (GunDisplay &gd : gundisplays) {
      gd.draw(assets);
    }
  }
  void update(GameRunner *r, SoundManager &s) {
    for (GunDisplay &g : gundisplays) {
      g.update(r, s);
    }
  };
};

class ShopMenu {
private:
  GUIButton finish_button =
      GUIButton(10, 10, 100, 40, GRAY, DARKGRAY, false,
                GUIText(60, 20, 30, "Finish", BLACK, true), true);

  GUIButton main_menu_button =
      GUIButton(10, 55, 100, 40, GRAY, DARKGRAY, false,
                GUIText(60, 65, 30, "Menu", BLACK, true), true);

  GUIText gold_text = GUIText(140, 190, 16, "0", WHITE, false);

  GUIText hpotions_text = GUIText(140, 210, 16, "0", WHITE, false);

  GUIText hpotions_tooltip =
      GUIText(120, 150, 12, "Restores\n50hp", GREEN, false);

  GUIButton hpotion_button =
      GUIButton(120, 130, 32, 16, GRAY, DARKGRAY, false,
                GUIText(122, 130, 16, "Buy", BLACK, false));

  GUIButton upgradesmenu_button =
      GUIButton(178, 5, 124, 16, GRAY, DARKGRAY, false,
                GUIText(178 + 124 / 2, 4, 16, "Upgrades", BLACK, true));
  GUIButton gunsmenu_button =
      GUIButton(178, 25, 124, 16, GRAY, DARKGRAY, false,
                GUIText(178 + 124 / 2, 25, 16, "Guns", BLACK, true));

  ShopState state = SHOP_STATE_UPGRADES;

  ShopMenuUpgrades upgradesmenu;

  GameRunner *runner;

  std::string gtxt;
  std::string hptxt;

public:
  ShopMenuGuns gunsmenu;
  void setRunner(GameRunner *runner) { this->runner = runner; }
  ShopMenu() : ShopMenu(nullptr) {};
  ShopMenu(GameRunner *runner) {
    upgradesmenu.current_upgrade.index = "goldmultiplier";
    upgradesmenu.current_upgrade.texid = "u_goldmultiplier";
    setRunner(runner);
    gunsmenu = ShopMenuGuns(this->runner);
  }

  void draw_health_bar() {
    int x = 15;
    int y = 205;
    int w = 90;
    int h = 10;
    int padding = 2;

    DrawRectangle(x, y, w, h, BLACK);
    DrawRectangle(x + padding, y + padding, w - padding * 2, h - padding * 2,
                  RED);
    int health_width = (w - padding * 2) * (runner->p.health / 100.0f);
    DrawRectangle(x + padding, y + padding, health_width, h - padding * 2,
                  GREEN);
  };

  void draw_player_stuff(GameAssets &assets, int frame) {
    DrawTexturePro(
        *assets.fetchTexture("crab"), {(float)16 * frame, (float)0.0f, 16, 16},
        {(float)15, (float)110, (float)90, (float)90}, {0, 0}, 0, RAYWHITE);
    draw_health_bar();
  }

  void draw_hpotion_buy(GameAssets &assets) {
    DrawTexturePro(*assets.fetchTexture("hpotion"), {0, 0, 8, 8},
                   {120, 110, 16, 16}, {0, 0}, 0.0f, WHITE);
    DrawText("=", 120 + 16 + 2, 110, 22, WHITE);
    DrawText("5", 120 + 16 + 2 + MeasureText("5", 16) + 2, 110, 16, WHITE);

    int x = 150;
    int y = 116;

    if (hpotion_button.isMouseOn())
      hpotions_tooltip.draw();
  }

  void draw_inventory(GameAssets &assets) {
    gold_text.draw();
    DrawTexturePro(*assets.fetchTexture("gold"), {0, 0, 8, 8},
                   {120, 190, 16, 16}, {0, 0}, 0.0f, WHITE);

    hpotions_text.draw();
    DrawTexturePro(*assets.fetchTexture("hpotion"), {0, 0, 8, 8},
                   {120, 210, 16, 16}, {0, 0}, 0.0f, WHITE);

    draw_hpotion_buy(assets);
  }

  void draw_current(GameAssets &assets) {
    switch (state) {
    case SHOP_STATE_UPGRADES:
      upgradesmenu.draw(assets);
      break;
    case SHOP_STATE_GUNS:
      gunsmenu.draw(runner, assets);
      break;
    }
  }

  void draw(GameAssets &assets, int frame, Upgrades &upgrades) {
    grain(assets);

    finish_button.draw();
    main_menu_button.draw();
    hpotion_button.draw();
    upgradesmenu_button.draw();
    gunsmenu_button.draw();
    DrawRectangle(10, 110, 100, 115, DARKBLUE);
    EndShaderMode();

    draw_current(assets);

    draw_player_stuff(assets, frame);
    draw_inventory(assets);
  };

  void update(SoundManager &s_manager, Inventory &inv, Upgrades &upgrades,
              GameState &state) {
    gtxt = std::to_string(inv.getGold());
    gold_text.setValue(gtxt.c_str());
    hptxt = std::to_string(inv.gethPotions());
    hpotions_text.setValue(hptxt.c_str());

    upgradesmenu.update(upgrades, inv, s_manager);
    gunsmenu.update(runner, s_manager);

    hpotion_button.setColor(5 <= inv.getGold() ? GREEN : RED);
    hpotion_button.setHoverColor(5 <= inv.getGold() ? GREEN : RED);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      if (hpotion_button.isMouseOn() && 5 <= inv.getGold()) {
        s_manager.play("select");
        inv.removeGold(5);
        inv.addHPotions(1);
      }

      if (finish_button.isMouseOn()) {
        s_manager.play("select");
        state = GAME_STATE_IN_GAME;
      }

      if (main_menu_button.isMouseOn()) {
        s_manager.play("select");
        state = GAME_STATE_MAIN_MENU;
      }

      if (gunsmenu_button.isMouseOn()) {
        s_manager.play("select");
        this->state = SHOP_STATE_GUNS;
      }
      if (upgradesmenu_button.isMouseOn()) {
        s_manager.play("select");
        this->state = SHOP_STATE_UPGRADES;
      }
    }
  }
};
