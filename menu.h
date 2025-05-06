#ifndef MENU_H
#define MENU_H

#include <SDL.h>
#include <SDL_ttf.h>
#include "graphics.h"
#include "audio.h"
#include <vector>
#include <variant>

enum GameState {
    HOMEPLAY, // Sảnh chính
    PLAYING,  // Trò chơi
    MAIN_MENU, // Menu chính
    GAME_OVER // Kết thúc trò chơi
};

enum MenuState {
    NONE,    // Không hiển thị menu
    MENU,    // Menu chính (menu.png)
    OPTIONS, // Menu tùy chọn (options.png)
    REVIVE   // Màn hình hồi sinh (revive.png)
};

class GameLoop;

class Menu {
public:
    Menu(Graphics& graphics, GameLoop& gameLoop, AudioManager& audioManager);
    ~Menu();

    void handleEvents(SDL_Event& e, GameState& gameState, MenuState& menuState, bool& isPaused);
    void updateReviveCountdown(GameState& gameState, MenuState& menuState, bool& isPaused);
    void render(Graphics& graphics, GameState gameState, MenuState menuState, bool& isPaused);
    bool isMenuVisible() const { return menuVisible; }
    void togglePause(GameState& gameState, MenuState& menuState, bool& isPaused);
    void startCountdown();
    void startReviveCountdown();

private:
    SDL_Texture* createCountdownTexture(Graphics& graphics, int value);

    SDL_Texture* homeplayTexture;
    SDL_Texture* menuTexture;
    SDL_Texture* offTexture;
    SDL_Texture* onTexture;
    SDL_Texture* optionsTexture;
    SDL_Texture* backTexture;
    SDL_Texture* reviveTexture;

    GameLoop& gameLoop;
    AudioManager& audioManager;
    bool menuVisible;
    bool isOffButtonActive;
    bool isCountingDown;
    int countdownValue;
    Uint32 countdownStartTime;
    bool isReviveCountingDown;
    float reviveCountdownValue; // Thời gian đếm ngược (giây, ví dụ: 5.0f)
    Uint32 reviveCountdownStartTime;
    using State = std::variant<GameState, MenuState>;
    std::vector<State> stateHistory;
};

#endif // MENU_H
