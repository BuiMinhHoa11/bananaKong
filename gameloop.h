#ifndef GAMELOOP_H
#define GAMELOOP_H

#include "common_func.h"
#include "graphics.h"
#include "obstacle.h"
#include "player.h"
#include "platform.h"
#include "banana.h"
#include "menu.h" // Đảm bảo bao gồm menu.h để có GameState
#include "audio.h"

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

    int getDistance() const { return distance; }
    int getBestDistance() const { return bestDistance; }
    int getCurrentBananas() const { return currentBananas; }
    int getTotalBananas() const { return totalBananas; }
    int getBestBananas() const { return bestBananas; }

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
    float scoreTimer;
    int distance;
    int bestDistance;
    int currentBananas;
    int totalBananas;
    int bestBananas;
};

#endif // GAMELOOP_H
