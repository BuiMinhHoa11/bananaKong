#include "graphics.h"

const char* KONGRUN_SPRITE_FILE = "D:/projectBTL/bananakong/image/CHAR/kong_run.png";
const int KONGRUN_CLIPS[][4] = {
    {   0, 0, 200, 149},
    { 200, 0, 200, 149},
    { 400, 0, 200, 149},
    { 600, 0, 200, 149},
    { 800, 0, 200, 149},
    {1000, 0, 200, 149},
    {1200, 0, 200, 149},
    {1400, 0, 200, 149},
    {1600, 0, 200, 149},
    {1800, 0, 200, 149},
    {2000, 0, 200, 149},
    {2200, 0, 200, 149},
    {2400, 0, 200, 149}
};
const int KONGRUN_FRAMES = sizeof(KONGRUN_CLIPS)/sizeof(int)/4;

const char* KONGFLY_SPRITE_FILE = "D:/projectBTL/bananakong/image/CHAR/kong_fly.png";
const int KONGFLY_CLIPS[][4] = {
    {0, 0, 200, 150} // 1 frame
};
const int KONGFLY_FRAMES = sizeof(KONGFLY_CLIPS)/sizeof(int)/4;

Graphics::Graphics(SDL_Renderer* renderer) : renderer(renderer) {}

Graphics::~Graphics() {}

SDL_Texture* Graphics::loadTexture(const char* filename) {
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);
    SDL_Texture* texture = IMG_LoadTexture(renderer, filename);
    if (texture == NULL) {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Load texture %s", IMG_GetError());
    }
    return texture;
}

void Graphics::renderTexture(SDL_Texture* texture, int x, int y) {
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
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

void Graphics::render(const ScrollingBackground& bgr) {
    int x = bgr.scrollingOffset;
    while (x > -bgr.width) {
        renderTexture(bgr.texture, x, 0);
        x -= bgr.width;
    }
}
