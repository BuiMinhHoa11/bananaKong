#ifndef COMMON_FUNC__H_
#define COMMON_FUNC__H_

#include <bits/stdc++.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

using namespace std;

extern SDL_Window* window;
extern SDL_Renderer* renderer;

const int SCREEN_WIDTH = 1500;
const int SCREEN_HEIGHT = 963;
const int SCREEN_BPP = 32;
const string WINDOW_TITLE = "BANANA KONG";

const int GROUND_LEVEL = 885;
const int KONG_HEIGHT = 150;
const int KONG_DRAW_Y_START = GROUND_LEVEL - KONG_HEIGHT; // 888 - 150 = 738

void initSDL(SDL_Window* &window, SDL_Renderer* &renderer);
void logSDLError(ostream& os, const string &msg, bool fatal = false);
void quitSDL(SDL_Window* window, SDL_Renderer* renderer);
void waitUntilKeyPressed();

void initTTF();
SDL_Texture* createTextTexture(SDL_Renderer* renderer, const char* text, TTF_Font* font, SDL_Color color, int& width, int& height);

#endif // COMMON_FUNC__H_
