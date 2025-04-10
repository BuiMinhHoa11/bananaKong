#include "common_func.h"
#include "graphics.h"

int main(int argc, char* argv[]){
    initSDL(window, renderer);
    Graphics graphics(renderer);
    //nền lớp xa
    ScrollingBackground backgroundSky;
    SDL_Texture* skyTexture = graphics.loadTexture("D:/HocLamGameSDLbananakong/bananakong/image/ITEM_BACK/sky.png");
    if (skyTexture == NULL) {
        SDL_Log("Failed to load sky texture!");
    } else {
        SDL_Log("Sky texture loaded successfully!");
    }
    backgroundSky.setTexture(skyTexture);

    //nền lớp gần
    ScrollingBackground background;
    SDL_Texture* backgroundTexture = graphics.loadTexture("D:/HocLamGameSDLbananakong/bananakong/image/ITEM_BACK/ground1.png");
    if (backgroundTexture == NULL) {
        SDL_Log("Failed to load ground_trees texture!");
    } else {
        SDL_Log("Ground_trees texture loaded successfully!");
    }
    background.setTexture(backgroundTexture);

    //stay
    /*SDL_Texture* background = graphics.loadTexture("D:/HocLamGameSDLbananakong/bananakong/image/ITEM_BACK/ground.png");
    graphics.renderTexture(background, 0, 0);
    graphics.presentScene();
    waitUntilKeyPressed();*/

    //move
    Sprite kongrun;
    SDL_Texture* kongrunTexture = graphics.loadTexture(KONGRUN_SPRITE_FILE);
    kongrun.init(kongrunTexture, KONGRUN_FRAMES, KONGRUN_CLIPS);

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
            backgroundSky.scroll(8);
            background.scroll(15); // Cuộn nền


        }
        graphics.prepareScene(); // Xóa màn hình
        // Vẽ lớp bầu trời (lớp xa) trước
        graphics.render(backgroundSky); // Vẽ toàn màn hình
        // Vẽ lớp gần
        graphics.render(background);
        graphics.render(120, 755, kongrun); // Vẽ nhân vật kongrun
        graphics.presentScene(); // Hiển thị khung hình
        SDL_Delay(100);
    }

    SDL_DestroyTexture( kongrunTexture ); kongrunTexture = NULL;
    SDL_DestroyTexture(skyTexture); skyTexture = NULL;
    SDL_DestroyTexture( backgroundTexture ); backgroundTexture = NULL;
    quitSDL(window, renderer);
    return 0;
}
