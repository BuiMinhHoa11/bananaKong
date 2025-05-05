#ifndef MENU_H
#define MENU_H

#include <SDL.h>
#include "graphics.h"

enum GameState {
    HOMEPLAY,
    PLAYING,
    MENU,
    GAME_OVER
};

class GameLoop;

class Menu {
public:
    Menu(Graphics& graphics, GameLoop& gameLoop);
    ~Menu();

    void handleEvents(SDL_Event& e, GameState& gameState, bool& isPaused);
    void render(Graphics& graphics, GameState gameState, bool& isPaused);
    bool isMenuVisible() const { return menuVisible; }
    void togglePause(GameState& gameState, bool& isPaused);
    void startCountdown();

private:
    SDL_Texture* createCountdownTexture(Graphics& graphics, int value);

    SDL_Texture* homeplayTexture;
    SDL_Texture* menuTexture;
    SDL_Texture* offTexture;
    SDL_Texture* onTexture;

    GameLoop& gameLoop;
    bool menuVisible;

    bool isOffButtonActive;  // true khi hiển thị nút OFF, false khi hiển thị nút ON
    bool isCountingDown;     // true khi đang đếm ngược
    int countdownValue;      // giá trị đếm ngược hiện tại (3,2,1)
    Uint32 countdownStartTime; // thời điểm bắt đầu đếm ngược
};

#endif // MENU_H
