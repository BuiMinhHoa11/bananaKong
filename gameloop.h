#ifndef GAMELOOP_H
#define GAMELOOP_H

#include "graphics.h"
#include "player.h"
#include "obstacle.h"
#include "platform.h"
#include "banana.h"
#include "audio.h"
#include "menu.h"

class ScrollingBackground;

class GameLoop {
public:
    GameLoop(Graphics& graphics, Player& player, ObstacleManager& obstacleManager,
             PlatformManager& platformManager, BananaManager& bananaManager,
             ScrollingBackground& backgroundSky, ScrollingBackground& background,
             ScrollingBackground& leafTop, AudioManager& audioManager);
    void handleEvents(SDL_Event& e, GameState& gameState);
    void update(GameState& gameState, float deltaTime);
    void render(Graphics& graphics, GameState gameState, TTF_Font* font);
    void reset();
    void revivePlayer();

    int getCurrentBananas() const { return currentBananas; }
    int getTotalBananas() const { return totalBananas; }
    int getBestBananas() const { return bestBananas; }
    int getLastGameBananas() const { return lastGameBananas; } // Thêm phương thức lấy điểm trận cuối

private:
    Player& kong;
    ObstacleManager& obstacleManager;
    PlatformManager& platformManager;
    BananaManager& bananaManager;
    ScrollingBackground& backgroundSky;
    ScrollingBackground& background;
    ScrollingBackground& leafTop;
    AudioManager& audioManager;
    bool isSpaceHeld;
    Uint32 lastFrameTime;
    float animationUpdateTimer;
    int currentBananas;
    int totalBananas;
    int bestBananas;
    int lastGameBananas; // Thêm biến lưu điểm trận cuối
};

#endif // GAMELOOP_H
