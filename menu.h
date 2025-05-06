#ifndef MENU_H
#define MENU_H

#include <SDL.h>
#include "graphics.h"
#include "audio.h"
#include <vector>
#include <variant>

enum GameState {
    HOMEPLAY, // Sảnh chính
    PLAYING,  // Trò chơi
    MAIN_MENU, // Thay thế MENU để tránh xung đột
    GAME_OVER // Kết thúc trò chơi
};

enum MenuState {
    NONE,    // Không hiển thị menu
    MENU,    // Bảng menu.png
    OPTIONS  // Bảng options.png
};

class GameLoop;

class Menu {
public:
    Menu(Graphics& graphics, GameLoop& gameLoop, AudioManager& audioManager);
    ~Menu();

    void handleEvents(SDL_Event& e, GameState& gameState, MenuState& menuState, bool& isPaused);
    void render(Graphics& graphics, GameState gameState, MenuState menuState, bool& isPaused);
    bool isMenuVisible() const { return menuVisible; }
    void togglePause(GameState& gameState, MenuState& menuState, bool& isPaused);
    void startCountdown();

private:
    SDL_Texture* createCountdownTexture(Graphics& graphics, int value);

    SDL_Texture* homeplayTexture;
    SDL_Texture* menuTexture;
    SDL_Texture* offTexture;
    SDL_Texture* onTexture;
    SDL_Texture* optionsTexture;
    SDL_Texture* backTexture;

    GameLoop& gameLoop;
    AudioManager& audioManager;
    bool menuVisible;
    bool isOffButtonActive;
    bool isCountingDown;
    int countdownValue;
    Uint32 countdownStartTime;
    using State = std::variant<GameState, MenuState>;
    std::vector<State> stateHistory;
};

#endif // MENU_H
