#include "common_func.h"
#include "graphics.h"
#include "obstacle.h"
#include "player.h"
#include "platform.h"
#include "banana.h"
#include "gameloop.h"
#include "menu.h"
#include "audio.h"

int main(int argc, char* argv[]) {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    initSDL(window, renderer);
    initTTF();
    Graphics graphics(renderer);

    // Khởi tạo AudioManager
    AudioManager audioManager;
    if (!audioManager.init()) {
        SDL_Log("Failed to initialize audio: %s", Mix_GetError());
        quitSDL(window, renderer);
        return 1;
    }
    audioManager.loadSounds();

    TTF_Font* font = TTF_OpenFont("D:/projectBTL/bananakong/font/Gameplay.ttf", 24);
    if (!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        quitSDL(window, renderer);
        return 1;
    }

    ScrollingBackground backgroundSky;
    SDL_Texture* skyTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/skymany.png");
    backgroundSky.setTexture(skyTexture);

    ScrollingBackground background;
    SDL_Texture* backgroundTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/ground1.png");
    background.setTexture(backgroundTexture);

    ScrollingBackground leafTop;
    SDL_Texture* leafTopTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/leafTop.png");
    leafTop.setTexture(leafTopTexture);

    Player kong(audioManager); // Đã sửa từ Player kong;
    SDL_Texture* kongrunTexture = graphics.loadTexture(KONGRUN_SPRITE_FILE);
    SDL_Texture* kongflyTexture = graphics.loadTexture(KONGFLY_SPRITE_FILE);
    SDL_Texture* kongdieTexture = graphics.loadTexture(KONGDIE_SPRITE_FILE);
    kong.init(kongrunTexture, kongflyTexture, kongdieTexture);
    kong.setPosition(400, KONG_DRAW_Y_START);

    std::map<ObstacleType, SDL_Texture*> obstacleTextures;
    obstacleTextures[ObstacleType::ROCK] = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/daHeo.png");
    obstacleTextures[ObstacleType::SPIKE] = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/cot.png");
    obstacleTextures[ObstacleType::PLANE] = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/plane.png");
    ObstacleManager obstacleManager(obstacleTextures);

    std::map<PlatformType, SDL_Texture*> platformTextures;
    platformTextures[PlatformType::GRASS_BIG] = graphics.loadTexture("D:/projectBTL/bananakong/image/PLATFORM/grass_big.png");
    platformTextures[PlatformType::GRASS_MID] = graphics.loadTexture("D:/projectBTL/bananakong/image/PLATFORM/grass_mid.png");
    platformTextures[PlatformType::GRASS_SUPERBIG] = graphics.loadTexture("D:/projectBTL/bananakong/image/PLATFORM/grass_superbig.png");
    platformTextures[PlatformType::LAND_MID] = graphics.loadTexture("D:/projectBTL/bananakong/image/PLATFORM/land_mid.png");
    platformTextures[PlatformType::LAND_SMALL] = graphics.loadTexture("D:/projectBTL/bananakong/image/PLATFORM/land_small.png");
    PlatformManager platformManager(platformTextures, obstacleManager);

    std::map<BananaType, SDL_Texture*> bananaTextures;
    bananaTextures[BananaType::NORMAL] = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/banana.png");
    BananaManager bananaManager(bananaTextures, platformManager, obstacleManager);

    GameLoop gameLoop(graphics, kong, obstacleManager, platformManager, bananaManager, backgroundSky, background, leafTop, audioManager);
    Menu menu(graphics, gameLoop, audioManager);

    bool quit = false;
    SDL_Event e;
    GameState gameState = HOMEPLAY;
    MenuState menuState = NONE;
    bool isPaused = false;

    // Phát nhạc nền ban đầu
    audioManager.playMusic(MusicType::HOMEPLAY);

    Uint32 lastFrameTime = SDL_GetTicks();
    while (!quit) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastFrameTime) / 1000.0f;
        lastFrameTime = currentTime;

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            menu.handleEvents(e, gameState, menuState, isPaused);
            if (!isPaused && gameState == PLAYING) {
                gameLoop.handleEvents(e, gameState);
            }
        }

        // Cập nhật nhạc nền theo trạng thái game
        if (gameState == HOMEPLAY && !isPaused) {
            audioManager.playMusic(MusicType::HOMEPLAY);
        } else if (gameState == PLAYING && !isPaused) {
            audioManager.playMusic(MusicType::LOOP);
        } else if (gameState == GAME_OVER || isPaused) {
            audioManager.stopMusic();
        }

        if (!isPaused && gameState == PLAYING) {
            gameLoop.update(gameState, deltaTime);
        }

        graphics.prepareScene();
        if (gameState == PLAYING || gameState == GAME_OVER) {
            gameLoop.render(graphics, gameState, font);
        }
        menu.render(graphics, gameState, menuState, isPaused);
        graphics.presentScene();
        SDL_Delay(16);
    }

    TTF_CloseFont(font);
    TTF_Quit();

    SDL_DestroyTexture(kongrunTexture);
    SDL_DestroyTexture(kongflyTexture);
    SDL_DestroyTexture(kongdieTexture);
    for (auto& [type, tex] : platformTextures) {
        SDL_DestroyTexture(tex);
    }
    for (auto& [type, tex] : obstacleTextures) {
        SDL_DestroyTexture(tex);
    }
    for (auto& [type, tex] : bananaTextures) {
        SDL_DestroyTexture(tex);
    }
    SDL_DestroyTexture(leafTopTexture);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(skyTexture);
    quitSDL(window, renderer);
    return 0;
}
