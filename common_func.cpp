#include "common_func.h"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

void initSDL(SDL_Window* &window, SDL_Renderer* &renderer) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        logSDLError(std::cout, "SDL_Init", true);
    }

    window = SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        logSDLError(std::cout, "CreateWindow", true);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        logSDLError(std::cout, "CreateRenderer", true);
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void logSDLError(ostream& os, const string &msg, bool fatal) {
    os << msg << " Error: " << SDL_GetError() << endl;
    if (fatal) {
        SDL_Quit();
        exit(1);
    }
}

void quitSDL(SDL_Window* window, SDL_Renderer* renderer) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void waitUntilKeyPressed() {
    SDL_Event e;
    while (true) {
        if (SDL_WaitEvent(&e) != 0 && (e.type == SDL_KEYDOWN || e.type == SDL_QUIT))
            return;
        SDL_Delay(100);
    }
}

void initTTF() {
    if (TTF_Init() == -1) {
        std::cout << "TTF_Init Error: " << TTF_GetError() << std::endl;
        exit(1);
    }
}

SDL_Texture* createTextTexture(SDL_Renderer* renderer, const char* text, TTF_Font* font, SDL_Color color, SDL_Rect& size) {
    if (!font || !text) return nullptr;

    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    if (!surface) {
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return nullptr;
    }

    size.x = 0;
    size.y = 0;
    size.w = surface->w;
    size.h = surface->h;

    SDL_FreeSurface(surface);
    return texture;
}
