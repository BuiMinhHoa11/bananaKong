#ifndef GRAPHICS__H_
#define GRAPHICS__H_

#include "common_func.h"

extern const char* KONGRUN_SPRITE_FILE;
extern const int KONGRUN_CLIPS[][4];
extern const int KONGRUN_FRAMES;

//scrolling background
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
        if( scrollingOffset < 0 ) { scrollingOffset = width; }
    }
};

//char animation
struct Sprite {
    SDL_Texture* texture;
    std::vector<SDL_Rect> clips;
    int currentFrame = 0;

    void init(SDL_Texture* _texture, int frames, const int _clips [][4]) {
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
        currentFrame = (currentFrame + 1) % clips.size();
    }

    const SDL_Rect* getCurrentClip() const {
        return &(clips[currentFrame]);
    }
};

class Graphics {
public:
    Graphics(SDL_Renderer* renderer);
    ~Graphics();

    SDL_Texture* loadTexture(const char *filename);
    void renderTexture(SDL_Texture *texture, int x, int y);
    void render(int x, int y, const Sprite& sprite);
    void prepareScene();
    void presentScene();
    // Thêm hàm render cho ScrollingBackground
    void render(const ScrollingBackground& bgr);
private:
    SDL_Renderer* renderer;
};

/*struct Obstacle {
    SDL_Rect rect;
    SDL_Texture* texture;

    void move(int speed) {
        rect.x -= speed;
    }

    bool isOffScreen() const {
        return rect.x + rect.w < 0;
    }
};*/

//sinh chướng ngại vật
//void spawnObstacle(vector<Obstacle>& obstacles, SDL_Texture* texture);

enum class ObstacleType {
    ROCK,
    SPIKE,
    LOG
};

struct Obstacle {
    SDL_Rect rect;
    SDL_Texture* texture;
    ObstacleType type;

    void move(int speed) {
        rect.x -= speed;
    }

    bool isOffScreen() const {
        return rect.x + rect.w < 0;
    }
};



// Khai báo
void spawnObstacle(vector<Obstacle>& obstacles, const map<ObstacleType, SDL_Texture*>& textureMap);
void spawnObstacleSingle(Obstacle& obs, const map<ObstacleType, SDL_Texture*>& textureMap, int offsetX = 0);


#endif // GRAPHICS__H_
