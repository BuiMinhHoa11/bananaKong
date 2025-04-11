#include "common_func.h"
#include "graphics.h"
#include "collision.h"

int main(int argc, char* argv[]){
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

    //chướng ngại vật
    vector<Obstacle> obstacles;
    Uint32 lastSpawnTime = SDL_GetTicks();
    const int spawnInterval = 15000;// 15 giây
          //SDL_Texture* obstacleTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/daHeo.png");
    map<ObstacleType, SDL_Texture*> obstacleTextures;
    obstacleTextures[ObstacleType::ROCK] = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/daHeo.png");
    obstacleTextures[ObstacleType::SPIKE] = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/cot.png");
    obstacleTextures[ObstacleType::LOG]   = graphics.loadTexture("D:/projectBTL/bananakong/image/ITEM_BACK/grass1.png");

    // Kiểm tra load thành công
    for (const auto& [type, tex] : obstacleTextures) {
        if (tex == nullptr) SDL_Log("Failed to load obstacle texture!");
    }

    bool quit = false;
    SDL_Event e;
    bool isGameStarted = false; // Trạng thái game: chưa bắt đầu

    while( !quit ) {
        while( SDL_PollEvent( &e ) != 0 ) {
            if( e.type == SDL_QUIT ) quit = true;
            if (e.type == SDL_MOUSEBUTTONDOWN && !isGameStarted) {
                    isGameStarted = true; // Bắt đầu game
            }
        }

        // Chỉ cập nhật chuyển động nếu game đã bắt đầu
        if (isGameStarted) {
            kongrun.tick(); // Animation của Kong
            backgroundSky.scroll(5);
            background.scroll(10);
            leafTop.scroll(30);

            //}

            // Spawn obstacle sau mỗi khoảng thời gian
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - lastSpawnTime > spawnInterval) {
                spawnObstacle(obstacles, obstacleTextures);
                lastSpawnTime = currentTime;
            }
            // Di chuyển obstacle
            for (auto& obs : obstacles) {
                obs.move(13); // trôi cùng tốc độ nền gần
            }
            // Xóa obstacle khi ra khỏi màn hình
            obstacles.erase(
                remove_if(obstacles.begin(), obstacles.end(),
                    [](const Obstacle& o) { return o.isOffScreen(); }),
                obstacles.end()
            );
        }

        graphics.prepareScene();// Xóa màn hình
        graphics.render(backgroundSky);// Vẽ lớp bottom
        graphics.render(background);// Vẽ lớp middle
        graphics.render(leafTop);// Vẽ lớp top
        // Vẽ obstacles
        for (const auto& obs : obstacles) {
            graphics.renderTexture(obs.texture, obs.rect.x, obs.rect.y);
        }
        graphics.render(120, 755, kongrun);// Vẽ nhân vật kongrun
        graphics.presentScene();// Hiển thị khung hình
        SDL_Delay(100);
    }

    SDL_DestroyTexture( kongrunTexture ); kongrunTexture = NULL;

    //SDL_DestroyTexture(obstacleTexture); obstacleTexture = NULL;
    for (auto& [type, tex] : obstacleTextures) {
        SDL_DestroyTexture(tex);
    }


    SDL_DestroyTexture(leafTopTexture); leafTopTexture = NULL;
    SDL_DestroyTexture( backgroundTexture ); backgroundTexture = NULL;
    SDL_DestroyTexture(skyTexture); skyTexture = NULL;

    quitSDL(window, renderer);
    return 0;
}
