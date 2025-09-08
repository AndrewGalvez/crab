#pragma once
#include "game_assets.hpp"
#include "game_state.hpp"
#include "gui.hpp"
#include "flex_gui.hpp"
#include "save.hpp"
#include "sound.hpp"
#include <raylib.h>
#include <functional>

class MainMenu {
private:
  FlexContainer titleContainer;
  FlexContainer buttonContainer;
  FlexContainer creditContainer;
  
  FlexText* titleText;
  FlexText* authorText;
  FlexText* wasmText;
  FlexText* musicCreditText;
  
  FlexButton* newGameButton;
  FlexButton* loadSaveButton;
  FlexButton* settingsButton;
  FlexButton* quitButton;

public:
  MainMenu() {
    // Setup title container
    titleContainer.setContainer(10, 10, 300, 80);
    titleContainer.setLayout(FlexContainer::VERTICAL);
    titleContainer.setSpacing(2);
    titleContainer.setPadding(0);
    titleContainer.setMargin(1);
    titleContainer.setDebugOverflow(false);
    
    titleText = titleContainer.add<FlexText>("Crab (2)", 48, WHITE);
    authorText = titleContainer.add<FlexText>("by Andy Galvez", 20, WHITE);
    
#ifdef __EMSCRIPTEN__
    wasmText = titleContainer.add<FlexText>("WASM: James Yacoube :] ", 16, Color{255, 107, 53, 255});
#endif
    
    // Setup button container with wrap layout - smaller margins and spacing
    buttonContainer.setContainer(10, 100, 300, 100);
    buttonContainer.setLayout(FlexContainer::WRAP);
    buttonContainer.setSpacing(5);  
    buttonContainer.setPadding(2);   
    buttonContainer.setMargin(3); 
    buttonContainer.setDebugOverflow(false);  
    
    newGameButton = buttonContainer.add<FlexButton>("NEW GAME", 20, GRAY, DARKGRAY, BLACK);  // Smaller font
    loadSaveButton = buttonContainer.add<FlexButton>("LOAD SAVE", 20, GRAY, DARKGRAY, BLACK);
    settingsButton = buttonContainer.add<FlexButton>("SETTINGS", 20, GRAY, DARKGRAY, BLACK);
    quitButton = buttonContainer.add<FlexButton>("QUIT GAME", 20, GRAY, DARKGRAY, BLACK);
    
    // Setup credit container
    creditContainer.setContainer(10, 215, 300, 25);
    creditContainer.setLayout(FlexContainer::VERTICAL);
    creditContainer.setPadding(0);
    creditContainer.setMargin(0);
    creditContainer.setDebugOverflow(false);  // Turn off debug now
    musicCreditText = creditContainer.add<FlexText>("Music: Fesliyan Studios - David Renda", 12, WHITE, false);
    
    // Layout all containers
    layoutElements();
  }
  
  void layoutElements() {
    titleContainer.layoutElements();
    buttonContainer.layoutElements();
    creditContainer.layoutElements();
    creditContainer.centerHorizontally();  // Center the credit text
  }
  void draw(GameAssets *game_assets, int frame) {
    // Smart layout - only recalculates when needed (screen size change, etc.)
    titleContainer.layoutElements();
    buttonContainer.layoutElements();
    creditContainer.layoutElements();
    creditContainer.centerHorizontally();  // Center the credit text
    
    // Set shader uniforms and begin shader mode (safe for WebGL)
    float time = floor(GetTime() * 9);
    Vector2 res = {320, 240};
    game_assets->setShaderValue("grain", "time", time);
    game_assets->setShaderValue("grain", "resolution", res);
    
    // Draw buttons with shader effect
    game_assets->beginShaderMode("grain");
    buttonContainer.draw();
    game_assets->endShaderMode("grain");
    
    // Draw text elements without shader
    titleContainer.draw();
    creditContainer.draw();
    
    // Draw animated crab sprite
    DrawTexturePro(*game_assets->fetchTexture("crab"),
                   {16 * (float)(frame % 4), 0, 16, 16},
                   {225, 0, 16 * 4, 16 * 4}, {0, 0}, 0.0f, WHITE);
  };

  void update(SoundManager &s_manager, GameState *state, bool **should_exit,
              SaveManager &s) {
    
    // Set up button callbacks
    newGameButton->setCallback([&]() {
      *state = GAME_STATE_IN_GAME;
      s_manager.play("select");
    });
    
    loadSaveButton->setCallback([&]() {
      s.load_game();
      *state = GAME_STATE_IN_GAME;
      s_manager.play("select");
    });
    
    settingsButton->setCallback([&]() {
      *state = GAME_STATE_SETTINGS;
      s_manager.play("select");
    });
    
    quitButton->setCallback([&]() {
      **should_exit = true;
      s_manager.play("select");
    });
    
    // Handle button clicks
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      buttonContainer.handleClick();
    }
  };
};
