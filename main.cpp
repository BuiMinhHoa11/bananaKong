#include "common_func.h"
#include "graphics.h"
#include "obstacle.h"
#include "player.h"
#include "platform.h"

enum GameState { MENU, PLAYING, GAME_OVER };

int main(int argc, char* argv[]) {
    initSDL(window, renderer);
    initTTF(); // Khởi tạo SDL_ttf
    Graphics graphics(renderer);

    // Tải font
    TTF_Font* font = TTF_OpenFont("D:/projectBTL/bananakong/font/Drawing_Kids.ttf", 24);
    if (!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        return 1;
    }
    SDL_Color white = {255, 255, 255, 255};

    // Nền lớp bottom
    ScrollingBackground backgroundSky;
    SDL_Texture* skyTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/skymany.png");
    if (skyTexture == NULL) {
        SDL_Log("Failed to load sky texture!");
    } else {
        SDL_Log("Sky texture loaded successfully!");
    }
    backgroundSky.setTexture(skyTexture);

    // Nền lớp middle
    ScrollingBackground background;
    SDL_Texture* backgroundTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/ground1.png");
    if (backgroundTexture == NULL) {
        SDL_Log("Failed to load ground_trees texture!");
    } else {
        SDL_Log("Ground_trees texture loaded successfully!");
    }
    background.setTexture(backgroundTexture);

    // Nền lớp top
    ScrollingBackground leafTop;
    SDL_Texture* leafTopTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/leafTop.png");
    if (leafTopTexture == NULL) {
        SDL_Log("Failed to load leaf_Top texture!");
    } else {
        SDL_Log("leaf_Top texture loaded successfully!");
    }
    leafTop.setTexture(leafTopTexture);

    // Khởi tạo nhân vật Player
    Player kong;
    SDL_Texture* kongrunTexture = graphics.loadTexture(KONGRUN_SPRITE_FILE);
    SDL_Texture* kongslideTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/CHAR/kong_slide.png");
    if (kongrunTexture == NULL) {
        SDL_Log("Failed to load kong run texture!");
    } else {
        SDL_Log("Kong run texture loaded successfully!");
    }
    if (kongslideTexture == NULL) {
        SDL_Log("Failed to load kong slide texture!");
    } else {
        SDL_Log("Kong slide texture loaded successfully!");
    }
    kong.init(kongrunTexture, kongslideTexture);
    kong.setPosition(120, 755);

    // Chướng ngại vật
    std::map<ObstacleType, SDL_Texture*> obstacleTextures;
    obstacleTextures[ObstacleType::ROCK] = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/daHeo.png");
    obstacleTextures[ObstacleType::SPIKE] = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/cot.png");
    obstacleTextures[ObstacleType::PLANE] = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/plane.png");
    for (const auto& [type, tex] : obstacleTextures) {
        if (tex == nullptr) SDL_Log("Failed to load obstacle texture!");
    }
    ObstacleManager obstacleManager(obstacleTextures);

    // Khởi tạo texture cho platform
    std::map<PlatformType, SDL_Texture*> platformTextures;
    platformTextures[PlatformType::GRASS_BIG] = graphics.loadTexture("D:/projectBTL/bananakong/image/PLATFORM/grass_big.png");
    platformTextures[PlatformType::GRASS_MID] = graphics.loadTexture("D:/projectBTL/bananakong/image/PLATFORM/grass_mid.png");
    platformTextures[PlatformType::GRASS_SUPERBIG] = graphics.loadTexture("D:/projectBTL/bananakong/image/PLATFORM/grass_superbig.png");
    platformTextures[PlatformType::LAND_MID] = graphics.loadTexture("D:/projectBTL/bananakong/image/PLATFORM/land_mid.png");
    platformTextures[PlatformType::LAND_SMALL] = graphics.loadTexture("D:/projectBTL/bananakong/image/PLATFORM/land_small.png");
    platformTextures[PlatformType::VINE] = graphics.loadTexture("D:/projectBTL/bananakong/image/PLATFORM/vine.png");

    // Kiểm tra tải texture
    for (const auto& [type, tex] : platformTextures) {
        if (tex == nullptr) SDL_Log("Failed to load platform texture!");
    }

    // Khởi tạo PlatformManager với cả platformTextures và obstacleManager
    PlatformManager platformManager(platformTextures, obstacleManager);

    bool quit = false;
    SDL_Event e;

    // Đo thời gian
    Uint32 lastFrameTime = SDL_GetTicks();
    float gameSpeedFactor = 0.5f; // 0.5 = 50% tốc độ ban đầu

    // Game over và khởi động lại
    GameState gameState = MENU; // Bắt đầu ở menu
    int score = 0;
    float scoreTimer = 0.0f;

    while (!quit) {
        // Xử lý events
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
                    if (e.key.keysym.sym == SDLK_s && kong.isOnGround()) {
                        kong.slide();
                    }
                    if (e.key.keysym.sym == SDLK_c) { // Phím C để bật/tắt khung va chạm
                        kong.toggleCollisionDisplay();
                    }
                }
                if (gameState == GAME_OVER && e.key.keysym.sym == SDLK_r) {
                    // Reset game
                    score = 0;
                    scoreTimer = 0.0f;
                    kong.setPosition(120, 755);
                    kong.setOnGround(true);
                    obstacleManager = ObstacleManager(obstacleTextures);
                    platformManager.clear();
                    gameState = PLAYING;
                }
            }
            if (e.type == SDL_KEYUP) {
                if (gameState == PLAYING && e.key.keysym.sym == SDLK_s) {
                    kong.stopSliding();
                }
            }
        }

        // Tính toán deltaTime
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastFrameTime) / 1000.0f;
        lastFrameTime = currentTime;

        // Chỉ cập nhật chuyển động nếu game đã bắt đầu
        if (gameState == PLAYING) {
            scoreTimer += deltaTime;
            if (scoreTimer >= 1.0f) {
                score += 100;
                scoreTimer -= 1.0f;
            }

            // Cập nhật nhân vật và chướng ngại vật
            vector<SDL_Rect> allPlatforms;
            // Thêm obstacles làm platform
            for (const auto& obs : obstacleManager.getObstacles()) {
                allPlatforms.push_back(obs.rect);
            }
            // Thêm platforms thực sự
            for (const auto& platform : platformManager.getPlatforms()) {
                allPlatforms.push_back(platform.rect);
            }
            kong.update(deltaTime, allPlatforms);

            backgroundSky.scroll(2 * gameSpeedFactor);
            background.scroll(5 * gameSpeedFactor);
            leafTop.scroll(15 * gameSpeedFactor);
            obstacleManager.update(deltaTime);
            platformManager.update(deltaTime);

            // Kiểm tra va chạm với chướng ngại vật (dùng khung tròn)
            SDL_Point center = kong.getCollisionCenter();
            int radius = kong.getCollisionRadius();
            if (obstacleManager.checkCollision(center.x, center.y, radius)) {
                gameState = GAME_OVER;
            }
        }

        // Render
        graphics.prepareScene();
        graphics.render(backgroundSky);
        graphics.render(background);
        graphics.render(leafTop);
        platformManager.render(&graphics);
        // Vẽ chướng ngại vật
        for (const auto& obs : obstacleManager.getObstacles()) {
            graphics.renderTexture(obs.texture, obs.rect.x, obs.rect.y);
        }

        // Vẽ khung va chạm của chướng ngại vật
        obstacleManager.renderDebugCollision(&graphics);
        // Vẽ Kong (bao gồm khung va chạm nếu showCollision = true)
        kong.render(&graphics);

        // Vẽ điểm
        std::string scoreText = "Score: " + std::to_string(score);
        int textW, textH;
        SDL_Texture* scoreTexture = createTextTexture(renderer, scoreText.c_str(), font, white, textW, textH);
        if (scoreTexture) {
            graphics.renderTexture(scoreTexture, 10, 10);
            SDL_DestroyTexture(scoreTexture);
        }

        // Vẽ màn hình game over
        if (gameState == GAME_OVER) {
            std::string gameOverText = "Game Over! Score: " + std::to_string(score) + " Press R to restart!!";
            SDL_Texture* gameOverTexture = createTextTexture(renderer, gameOverText.c_str(), font, white, textW, textH);
            if (gameOverTexture) {
                graphics.renderTexture(gameOverTexture, SCREEN_WIDTH / 2 - textW / 2, SCREEN_HEIGHT / 2 - textH / 2);
                SDL_DestroyTexture(gameOverTexture);
            }
        }

        graphics.presentScene();
        SDL_Delay(16); // Khoảng 60 FPS
    }

    // Dọn dẹp
    TTF_CloseFont(font);
    TTF_Quit();

    SDL_DestroyTexture(kongrunTexture);
    SDL_DestroyTexture(kongslideTexture);
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
