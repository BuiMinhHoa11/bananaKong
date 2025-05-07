#include "gameloop.h"

GameLoop::GameLoop(Graphics& graphics, Player& player, ObstacleManager& obstacleManager,
                   PlatformManager& platformManager, BananaManager& bananaManager,
                   ScrollingBackground& backgroundSky, ScrollingBackground& background,
                   ScrollingBackground& leafTop, AudioManager& audioManager)
    : kong(player), obstacleManager(obstacleManager), platformManager(platformManager),
      bananaManager(bananaManager), backgroundSky(backgroundSky), background(background),
      leafTop(leafTop), audioManager(audioManager), isSpaceHeld(false), lastFrameTime(SDL_GetTicks()),
      animationUpdateTimer(0.0f), currentBananas(0), totalBananas(0), bestBananas(0) {}

void GameLoop::handleEvents(SDL_Event& e, GameState& gameState) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    if (e.type == SDL_KEYDOWN) {
        if (gameState == GameState::PLAYING) {
            if (e.key.keysym.sym == SDLK_SPACE && !isSpaceHeld) {
                if (kong.isOnGround()) {
                    kong.jump();
                } else {
                    kong.startFly();
                    isSpaceHeld = true;
                }
            }
            if (e.key.keysym.sym == SDLK_s) {
                kong.climbDown(platformManager);
            }
            if (e.key.keysym.sym == SDLK_c) {
                kong.toggleCollisionDisplay();
            }
        }
        if (gameState == GameState::GAME_OVER && e.key.keysym.sym == SDLK_r) {
            currentBananas = 0;
            kong.setPosition(400, KONG_DRAW_Y_START);
            kong.setOnGround(true);
            kong.setState(PlayerState::RUN);
            obstacleManager.clear();
            platformManager.clear();
            bananaManager.clear();
            gameState = GameState::PLAYING;
        }
    }
    if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_SPACE && gameState == GameState::PLAYING) {
        isSpaceHeld = false;
        kong.stopFly();
    }
}

void GameLoop::update(GameState& gameState, float deltaTime) {
    if (gameState == GameState::PLAYING) {
        float difficulty = platformManager.getDifficulty();

        platformManager.update(deltaTime);
        obstacleManager.setDifficulty(platformManager.getDifficulty());
        obstacleManager.update(deltaTime, platformManager.getScrollSpeed());
        bananaManager.setScrollSpeed(platformManager.getScrollSpeed());
        bananaManager.setDifficultyFactor(platformManager.getDifficulty());
        bananaManager.update(deltaTime);

        std::vector<SDL_Rect> allPlatforms;
        for (const auto& platform : platformManager.getPlatforms()) {
            allPlatforms.push_back(platform.rect);
        }
        for (const auto& obs : obstacleManager.getObstacles()) {
            if (obs.isPlatform) {
                allPlatforms.push_back(obs.rect);
            }
        }
        kong.update(deltaTime, allPlatforms, platformManager);

        backgroundSky.scroll(static_cast<int>(platformManager.getScrollSpeed() * 0.4f));
        background.scroll(static_cast<int>(platformManager.getScrollSpeed() * 1.0f));
        leafTop.scroll(static_cast<int>(platformManager.getScrollSpeed() * 2.5f));

        animationUpdateTimer += deltaTime;
        if (animationUpdateTimer >= 1.0f) {
            float difficulty = platformManager.getDifficulty();
            int newFrameDelayMax = static_cast<int>(3.0f - (difficulty - 1.0f) * 0.2f);
            if (newFrameDelayMax < 1) newFrameDelayMax = 1;
            animationUpdateTimer = 0.0f;
        }

        SDL_Point center = kong.getCollisionCenter();
        int radius = kong.getCollisionRadius();
        if (obstacleManager.checkCollision(center.x, center.y, radius)) {
            if (kong.getState() == PlayerState::FLY) {
                kong.stopFly();
                kong.setOnGround(true);
            } else {
                kong.setState(PlayerState::DIE);
                audioManager.playSound(SoundType::DIE);
                gameState = GameState::GAME_OVER;
            }
        }

        bool magnetActivated = false;
        if (bananaManager.checkCollision(center.x, center.y, radius, totalBananas, magnetActivated)) {
            currentBananas++;
            audioManager.playSound(SoundType::EAT);
        }
    }
}

void GameLoop::render(Graphics& graphics, GameState gameState, TTF_Font* font) {
    graphics.render(backgroundSky);
    graphics.render(background);
    graphics.render(leafTop);
    platformManager.render(&graphics);
    for (const auto& obs : obstacleManager.getObstacles()) {
        graphics.renderTexture(obs.texture, obs.rect.x, obs.rect.y);
    }
    obstacleManager.renderDebugCollision(&graphics);
    bananaManager.render();
    bananaManager.renderDebugCollision(graphics.getRenderer());
    kong.render(&graphics);

    if (gameState == GameState::PLAYING) {
        SDL_Color white = {255, 255, 255, 255};
        int textW, textH;
        std::string bananasText = "Bananas: " + std::to_string(currentBananas);
        SDL_Texture* bananasTexture = createTextTexture(graphics.getRenderer(), bananasText.c_str(), font, white, textW, textH);
        if (bananasTexture) {
            graphics.renderTexture(bananasTexture, 10, 40);
            SDL_DestroyTexture(bananasTexture);
        }
    }
}

void GameLoop::reset() {
    currentBananas = 0;
    kong.setPosition(400, KONG_DRAW_Y_START);
    kong.setOnGround(true);
    kong.setState(PlayerState::RUN);
    obstacleManager.clear();
    platformManager.clear();
    bananaManager.clear();
}

void GameLoop::revivePlayer() {
    int kongX = kong.getX();
    obstacleManager.removeObstaclesNear(kongX, 600);
    kong.setPosition(kong.getX() + 600, KONG_DRAW_Y_START);
    kong.setOnGround(true);
    kong.setState(PlayerState::RUN);
}
