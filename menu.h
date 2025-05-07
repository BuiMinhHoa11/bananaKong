#ifndef MENU_H
#define MENU_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <stack>
#include <variant>
#include "graphics.h"
#include "audio.h"

// Khai báo trước lớp GameLoop
class GameLoop;

enum class GameState {
    HOMEPLAY,  // Sảnh chính
    PLAYING,   // Trò chơi
    MAIN_MENU, // Menu chính
    GAME_OVER  // Kết thúc trò chơi
};

enum class MenuState {
    NONE,
    MENU,
    OPTIONS,
    REVIVE,
    NEW_PANEL  // Trạng thái cho bảng Record
};

class Menu {
public:
    Menu(Graphics& graphics, GameLoop& gameLoop, AudioManager& audioManager);
    ~Menu();

    void handleEvents(SDL_Event& e, GameState& gameState, MenuState& menuState, bool& isPaused);
    void render(Graphics& graphics, GameState gameState, MenuState menuState, bool& isPaused);
    void togglePause(GameState& gameState, MenuState& menuState, bool& isPaused);
    void startCountdown();
    void startReviveCountdown();
    void updateReviveCountdown(GameState& gameState, MenuState& menuState, bool& isPaused);

private:
    GameLoop& gameLoop;
    AudioManager& audioManager;
    SDL_Texture* homeplayTexture;
    SDL_Texture* menuTexture;
    SDL_Texture* offTexture;
    SDL_Texture* onTexture;
    SDL_Texture* optionsTexture;
    SDL_Texture* backTexture;
    SDL_Texture* reviveTexture;
    SDL_Texture* musicOnTexture;    // Texture cho nút Music bật
    SDL_Texture* musicOffTexture;   // Texture cho nút Music tắt
    SDL_Texture* effectOnTexture;   // Texture cho nút Effect bật
    SDL_Texture* effectOffTexture;  // Texture cho nút Effect tắt
    SDL_Texture* recordTexture;     // Texture cho bảng Record
    TTF_Font* countdownFont;       // Phông chữ cho countdown (cỡ lớn)
    TTF_Font* reviveCountdownFont; // Phông chữ cho revive countdown (cỡ nhỏ)
    TTF_Font* recordFont;          // Phông chữ cho Record Panel
    bool menuVisible;
    bool isOffButtonActive;
    bool isCountingDown;
    int countdownValue;
    Uint32 countdownStartTime;
    bool isReviveCountingDown;
    float reviveCountdownValue;
    Uint32 reviveCountdownStartTime;
    std::stack<std::variant<GameState, MenuState>> stateHistory;
};

#endif // MENU_H
