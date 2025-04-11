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
const int SCREEN_BPP = 72;
const string WINDOW_TITLE = "BANANA KONG";

const int COLOR_KEY_R = 255;
const int COLOR_KEY_G = 255;
const int COLOR_KEY_B = 255;

const int RENDER_DRAW_COLOR = 0xff;

void initSDL(SDL_Window* &window, SDL_Renderer* &renderer);
void logSDLError(ostream& os, const string &msg, bool fatal = false);
void quitSDL(SDL_Window* window, SDL_Renderer* renderer);
void waitUntilKeyPressed();

#endif // COMMON_FUNC__H_
