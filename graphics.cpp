
#include "common_func.h"
#include "graphics.h"

const char* KONGRUN_SPRITE_FILE = "D:/HocLamGameSDLbananakong/bananakong/image/CHAR/kong_run.png";
const int KONGRUN_CLIPS[][4] = {
    {   0, 0, 200, 167},  // Frame 1
    { 200, 0, 200, 167},  // Frame 2
    { 400, 0, 200, 167},  // Frame 3
    { 600, 0, 200, 167},  // Frame 4
    { 800, 0, 200, 167},  // Frame 5
    {1000, 0, 200, 167},  // Frame 6
    {1200, 0, 200, 167},  // Frame 7
    {1400, 0, 200, 167},  // Frame 8
    {1600, 0, 200, 167},  // Frame 9
    {1800, 0, 200, 167},  // Frame 10
    {2000, 0, 200, 167},  // Frame 11
    {2200, 0, 200, 167},  // Frame 12
    {2400, 0, 200, 167}   // Frame 13
};
const int KONGRUN_FRAMES = sizeof(KONGRUN_CLIPS)/sizeof(int)/4;

Graphics::Graphics(SDL_Renderer* renderer) : renderer(renderer) {}

Graphics::~Graphics() {}

SDL_Texture* Graphics::loadTexture(const char *filename) {
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
                   "Loading %s", filename);

    SDL_Texture *texture = IMG_LoadTexture(renderer, filename);
    if (texture == NULL) {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
                       "Load texture %s", IMG_GetError());
    }

    return texture;
}

void Graphics::renderTexture(SDL_Texture *texture, int x, int y) {
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;

    // Truy vấn kích thước thực của texture
    int texWidth, texHeight;
    SDL_QueryTexture(texture, NULL, NULL, &texWidth, &texHeight);
    dest.w = texWidth;
    dest.h = texHeight;

    SDL_RenderCopy(renderer, texture, NULL, &dest);
}

void Graphics::render(int x, int y, const Sprite& sprite) {
    const SDL_Rect* clip = sprite.getCurrentClip();
    SDL_Rect renderQuad = {x, y, clip->w, clip->h};
    SDL_RenderCopy(renderer, sprite.texture, clip, &renderQuad);
}

void Graphics::prepareScene() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void Graphics::presentScene() {
    SDL_RenderPresent(renderer);
}

// Thêm hàm render cho ScrollingBackground
void Graphics::render(const ScrollingBackground& bgr) {
    int x = bgr.scrollingOffset;
    // Vẽ nền liên tục cho đến khi phủ kín màn hình
    while (x > -bgr.width) {
        renderTexture(bgr.texture, x, 0);
        x -= bgr.width;
    }
}
