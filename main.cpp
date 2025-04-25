#include "common_func.h"
#include "graphics.h"
#include "obstacle.h"
#include "player.h"
#include "platform.h"

enum GameState { MENU, PLAYING, GAME_OVER };

int main(int argc, char* argv[]) {
    initSDL(window, renderer);
    initTTF();
    Graphics graphics(renderer);

    TTF_Font* font = TTF_OpenFont("D:/projectBTL/bananakong/font/Drawing_Kids.ttf", 24);
    if (!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        return 1;
    }
    SDL_Color white = {255, 255, 255, 255};

    ScrollingBackground backgroundSky;
    SDL_Texture* skyTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/skymany.png");
    if (skyTexture == NULL) {
        SDL_Log("Failed to load sky texture!");
    } else {
        SDL_Log("Sky texture loaded successfully!");
    }
    backgroundSky.setTexture(skyTexture);

    ScrollingBackground background;
    SDL_Texture* backgroundTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/ground1.png");
    if (backgroundTexture == NULL) {
        SDL_Log("Failed to load ground_trees texture!");
    } else {
        SDL_Log("Ground_trees texture loaded successfully!");
    }
    background.setTexture(backgroundTexture);

    ScrollingBackground leafTop;
    SDL_Texture* leafTopTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/leafTop.png");
    if (leafTopTexture == NULL) {
        SDL_Log("Failed to load leaf_Top texture!");
    } else {
        SDL_Log("leaf_Top texture loaded successfully!");
    }
    leafTop.setTexture(leafTopTexture);

    Player kong;
    SDL_Texture* kongrunTexture = graphics.loadTexture(KONGRUN_SPRITE_FILE);
    if (kongrunTexture == NULL) {
        SDL_Log("Failed to load kong run texture!");
    } else {
        SDL_Log("Kong run texture loaded successfully!");
    }
    kong.init(kongrunTexture);
    kong.setPosition(400, 755);

    std::map<ObstacleType, SDL_Texture*> obstacleTextures;
    obstacleTextures[ObstacleType::ROCK] = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/daHeo.png");
    obstacleTextures[ObstacleType::SPIKE] = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/cot.png");
    obstacleTextures[ObstacleType::PLANE] = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/plane.png");
    for (const auto& [type, tex] : obstacleTextures) {
        if (tex == nullptr) SDL_Log("Failed to load obstacle texture!");
    }
    ObstacleManager obstacleManager(obstacleTextures);

    std::map<PlatformType, SDL_Texture*> platformTextures;
    platformTextures[PlatformType::GRASS_BIG] = graphics.loadTexture("D:/projectBTL/bananakong/image/PLATFORM/grass_big.png");
    platformTextures[PlatformType::GRASS_MID] = graphics.loadTexture("D:/projectBTL/bananakong/image/PLATFORM/grass_mid.png");
    platformTextures[PlatformType::GRASS_SUPERBIG] = graphics.loadTexture("D:/projectBTL/bananakong/image/PLATFORM/grass_superbig.png");
    platformTextures[PlatformType::LAND_MID] = graphics.loadTexture("D:/projectBTL/bananakong/image/PLATFORM/land_mid.png");
    platformTextures[PlatformType::LAND_SMALL] = graphics.loadTexture("D:/projectBTL/bananakong/image/PLATFORM/land_small.png");

    for (const auto& [type, tex] : platformTextures) {
        if (tex == nullptr) SDL_Log("Failed to load platform texture!");
    }

    PlatformManager platformManager(platformTextures, obstacleManager);

    bool quit = false;
    SDL_Event e;

    Uint32 lastFrameTime = SDL_GetTicks();
    float animationUpdateTimer = 0.0f;

    GameState gameState = MENU;
    int score = 0;
    float scoreTimer = 0.0f;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN && gameState == MENU) {
                gameState = PLAYING;
            }
            if (e.type == SDL_KEYDOWN) {
                if (gameState == PLAYING) {
                    if (e.key.keysym.sym == SDLK_SPACE && kong.isOnGround()) {
                        kong.jump();
                    }
                    if (e.key.keysym.sym == SDLK_c) {
                        kong.toggleCollisionDisplay();
                    }
                }
                if (gameState == GAME_OVER && e.key.keysym.sym == SDLK_r) {
                    score = 0;
                    scoreTimer = 0.0f;
                    kong.setPosition(400, 755);
                    kong.setOnGround(true);
                    obstacleManager.clear();
                    platformManager.clear();
                    gameState = PLAYING;
                }
            }
        }

        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastFrameTime) / 1000.0f;
        lastFrameTime = currentTime;

        if (gameState == PLAYING) {
            scoreTimer += deltaTime;
            if (scoreTimer >= 1.0f) {
                score += 100;
                scoreTimer -= 1.0f;
            }

            // Update platformManager first to ensure scrollSpeed is up-to-date
            platformManager.update(deltaTime);

            // Now update obstacleManager with the correct scrollSpeed
            float scrollSpeed = platformManager.getScrollSpeed();
            obstacleManager.setDifficulty(platformManager.getDifficulty());
            obstacleManager.update(deltaTime, scrollSpeed); // Fixed: Pass both arguments

            std::vector<SDL_Rect> allPlatforms;
            for (const auto& platform : platformManager.getPlatforms()) {
                allPlatforms.push_back(platform.rect);
            }
            for (const auto& obs : obstacleManager.getObstacles()) {
                if (obs.isPlatform) {
                    allPlatforms.push_back(obs.rect);
                }
            }
            kong.update(deltaTime, allPlatforms);

            backgroundSky.scroll(static_cast<int>(scrollSpeed * 0.4f));
            background.scroll(static_cast<int>(scrollSpeed * 1.0f));
            leafTop.scroll(static_cast<int>(scrollSpeed * 2.5f));

            animationUpdateTimer += deltaTime;
            if (animationUpdateTimer >= 1.0f) {
                float difficulty = platformManager.getDifficulty();
                int newFrameDelayMax = static_cast<int>(3.0f - (difficulty - 1.0f) * 0.2f);
                if (newFrameDelayMax < 1) newFrameDelayMax = 1;
                kong.setAnimationSpeed(newFrameDelayMax);
                animationUpdateTimer = 0.0f;
            }

            SDL_Point center = kong.getCollisionCenter();
            int radius = kong.getCollisionRadius();
            if (obstacleManager.checkCollision(center.x, center.y, radius)) {
                gameState = GAME_OVER;
            }
        }

        graphics.prepareScene();
        graphics.render(backgroundSky);
        graphics.render(background);
        graphics.render(leafTop);
        platformManager.render(&graphics);
        for (const auto& obs : obstacleManager.getObstacles()) {
            graphics.renderTexture(obs.texture, obs.rect.x, obs.rect.y);
        }
        obstacleManager.renderDebugCollision(&graphics);
        kong.render(&graphics);

        std::string scoreText = "Score: " + std::to_string(score);
        int textW, textH;
        SDL_Texture* scoreTexture = createTextTexture(renderer, scoreText.c_str(), font, white, textW, textH);
        if (scoreTexture) {
            graphics.renderTexture(scoreTexture, 10, 10);
            SDL_DestroyTexture(scoreTexture);
        }

        if (gameState == GAME_OVER) {
            std::string gameOverText = "Game Over! Score: " + std::to_string(score) + " Press R to recover!!";
            SDL_Texture* gameOverTexture = createTextTexture(renderer, gameOverText.c_str(), font, white, textW, textH);
            if (gameOverTexture) {
                graphics.renderTexture(gameOverTexture, SCREEN_WIDTH / 2 - textW / 2, SCREEN_HEIGHT / 2 - textH / 2);
                SDL_DestroyTexture(gameOverTexture);
            }
        }

        graphics.presentScene();
        SDL_Delay(16);
    }

    TTF_CloseFont(font);
    TTF_Quit();

    SDL_DestroyTexture(kongrunTexture);
    for (auto& [type, tex] : platformTextures) {
        SDL_DestroyTexture(tex);
    }
    for (auto& [type, tex] : obstacleTextures) {
        SDL_DestroyTexture(tex);
    }
    SDL_DestroyTexture(leafTopTexture);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(skyTexture);
    quitSDL(window, renderer);
    return 0;
}
