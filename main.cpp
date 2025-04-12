#include "common_func.h"
#include "graphics.h"
#include "collision.h"
#include "obstacle.h"

int main(int argc, char* argv[]) {
    initSDL(window, renderer);
    Graphics graphics(renderer);

    //nền lớp bottom
    ScrollingBackground backgroundSky;
    SDL_Texture* skyTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/skymany.png");
    if (skyTexture == NULL) {
        SDL_Log("Failed to load sky texture!");
    } else {
        SDL_Log("Sky texture loaded successfully!");
    }
    backgroundSky.setTexture(skyTexture);

    //nền lớp middle
    ScrollingBackground background;
    SDL_Texture* backgroundTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/ground1.png");
    if (backgroundTexture == NULL) {
        SDL_Log("Failed to load ground_trees texture!");
    } else {
        SDL_Log("Ground_trees texture loaded successfully!");
    }
    background.setTexture(backgroundTexture);

    //nền lớp top
    ScrollingBackground leafTop;
    SDL_Texture* leafTopTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/leafTop.png");
    if (leafTopTexture == NULL) {
        SDL_Log("Failed to load leaf_Top texture!");
    } else {
        SDL_Log("leaf_Top texture loaded successfully!");
    }
    leafTop.setTexture(leafTopTexture);

    //move
    Sprite kongrun;
    SDL_Texture* kongrunTexture = graphics.loadTexture(KONGRUN_SPRITE_FILE);
    kongrun.init(kongrunTexture, KONGRUN_FRAMES, KONGRUN_CLIPS);
    kongrun.setAnimationSpeed(3); // Đặt tốc độ animation (10 = chậm hơn so với mặc định 5)

    //chướng ngại vật
    std::map<ObstacleType, SDL_Texture*> obstacleTextures;
    obstacleTextures[ObstacleType::ROCK] = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/daHeo.png");
    obstacleTextures[ObstacleType::SPIKE] = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/cot.png");
    obstacleTextures[ObstacleType::LOG] = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/grass1.png");
    // Kiểm tra load thành công
    for (const auto& [type, tex] : obstacleTextures) {
        if (tex == nullptr) SDL_Log("Failed to load obstacle texture!");
    }
    // Khởi tạo bộ quản lý chướng ngại vật
    ObstacleManager obstacleManager(obstacleTextures);

    bool quit = false;
    SDL_Event e;
    bool isGameStarted = false; // Trạng thái game: chưa bắt đầu

    // Đo thời gian
    Uint32 lastFrameTime = SDL_GetTicks();
    float gameSpeedFactor = 0.5f; // 0.5 = 50% tốc độ ban đầu

    while (!quit) {
        // Xử lý events
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN && !isGameStarted) {
                isGameStarted = true; // Bắt đầu game
            }
        }

        // Tính toán deltaTime
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastFrameTime) / 1000.0f; // Chuyển sang giây
        lastFrameTime = currentTime;

        // Chỉ cập nhật chuyển động nếu game đã bắt đầu
        if (isGameStarted) {
            // Cập nhật animation và nền
            kongrun.tick();
            backgroundSky.scroll(2 * gameSpeedFactor);
            background.scroll(5 * gameSpeedFactor);
            leafTop.scroll(15 * gameSpeedFactor);

            // Cập nhật chướng ngại vật
            obstacleManager.update(deltaTime);
        }

        // Render
        graphics.prepareScene();

        // Vẽ các lớp nền
        graphics.render(backgroundSky);
        graphics.render(background);
        graphics.render(leafTop);

        // Vẽ obstacles
        for (const auto& obs : obstacleManager.getObstacles()) {
            graphics.renderTexture(obs.texture, obs.rect.x, obs.rect.y);
        }

        // Vẽ nhân vật
        graphics.render(120, 755, kongrun);

        // Hiển thị khung hình
        graphics.presentScene();

        // Giới hạn FPS để không chạy quá nhanh
        SDL_Delay(16); // Khoảng 60 FPS
    }

    // Giải phóng tài nguyên
    SDL_DestroyTexture(kongrunTexture);
    kongrunTexture = NULL;

    // Giải phóng texture chướng ngại vật
    for (auto& [type, tex] : obstacleTextures) {
        SDL_DestroyTexture(tex);
        tex = NULL;
    }

    SDL_DestroyTexture(leafTopTexture);
    leafTopTexture = NULL;

    SDL_DestroyTexture(backgroundTexture);
    backgroundTexture = NULL;

    SDL_DestroyTexture(skyTexture);
    skyTexture = NULL;

    quitSDL(window, renderer);
    return 0;
}
