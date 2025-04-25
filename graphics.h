#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "common_func.h"

// Hằng số cho sprite của Kong
extern const char* KONGRUN_SPRITE_FILE;
extern const int KONGRUN_CLIPS[][4];
extern const int KONGRUN_FRAMES;

// Scrolling background
struct ScrollingBackground {
    SDL_Texture* texture;
    int scrollingOffset = 0;
    int width, height;

    void setTexture(SDL_Texture* _texture) {
        texture = _texture;
        SDL_QueryTexture(texture, NULL, NULL, &width, &height);
    }

    void scroll(int distance) {
        scrollingOffset -= distance;
        if (scrollingOffset < 0) {
            scrollingOffset = width;
        }
    }
};

// Sprite cho animation
struct Sprite {
    SDL_Texture* texture;
    std::vector<SDL_Rect> clips;
    int currentFrame = 0;
    int frameDelay = 0;
    int frameDelayMax = 6;

    void init(SDL_Texture* _texture, int frames, const int _clips[][4]) {
        texture = _texture;
        SDL_Rect clip;
        for (int i = 0; i < frames; i++) {
            clip.x = _clips[i][0];
            clip.y = _clips[i][1];
            clip.w = _clips[i][2];
            clip.h = _clips[i][3];
            clips.push_back(clip);
        }
    }

    void tick() {
        frameDelay++;
        if (frameDelay >= frameDelayMax) {
            currentFrame = (currentFrame + 1) % clips.size();
            frameDelay = 0;
        }
    }

    void setAnimationSpeed(int speed) {
        frameDelayMax = speed;
    }

    const SDL_Rect* getCurrentClip() const {
        return &(clips[currentFrame]);
    }
};

class Graphics {
public:
    Graphics(SDL_Renderer* renderer);
    ~Graphics();
    SDL_Texture* loadTexture(const char* filename);
    void renderTexture(SDL_Texture* texture, int x, int y);
    void render(int x, int y, const Sprite& sprite);
    void prepareScene();
    void presentScene();
    void render(const ScrollingBackground& bgr);
    // Thêm phương thức getRenderer
    SDL_Renderer* getRenderer() const { return renderer; } // Trả về renderer

private:
    SDL_Renderer* renderer;
};

#endif // GRAPHICS__H_
