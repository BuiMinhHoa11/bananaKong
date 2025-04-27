#ifndef PLAYER_H_
#define PLAYER_H_
#include "common_func.h"
#include "graphics.h"
#include <vector>
#include <map>

class PlatformManager;

enum PlayerState {
    IDLE,
    RUNNING,
    JUMPING,
    FALLING,
    FLYING // Đổi từ FLY thành FLYING
};

class Player {
private:
    float x, y;
    float velocityX, velocityY;
    PlayerState state;
    float gravity;
    float jumpForce;
    int width, height;
    bool onGround;
    bool isClimbingDown;
    Sprite runSprite;
    Sprite flySprite; // Sprite cho FLYING (1 frame)
    int collisionRadius;
    bool showCollision;
    float flyStartY; // Lưu y khi bắt đầu FLYING

    bool checkPlatformCollision(const SDL_Rect& obstacle);

public:
    Player();
    ~Player();

    void init(SDL_Texture* runTexture, SDL_Texture* flyTexture);
    void update(float deltaTime, const std::vector<SDL_Rect>& platforms, PlatformManager& platformManager);
    void render(Graphics* graphics);
    void renderDebugCollision(Graphics* graphics);
    void renderCircularCollision(Graphics* graphics);

    void jump();
    void climbDown(PlatformManager& platformManager);
    void startFly(); // Đổi tên hàm cho thống nhất
    void stopFly();

    SDL_Rect getCollisionBox() const;
    SDL_Point getCollisionCenter() const;
    int getCollisionRadius() const;
    bool checkCircularCollision(const SDL_Point& otherCenter, int otherRadius) const;

    void setCollisionRadius(int radius);
    void setOnGround(bool grounded);
    bool isOnGround() const { return onGround; }

    float getX() const { return x; }
    float getY() const { return y; }
    void setPosition(float newX, float newY);

    PlayerState getState() const { return state; }

    void toggleCollisionDisplay();
    bool isCollisionDisplayed() const { return showCollision; }

    void setAnimationSpeed(int speed) {
        runSprite.setAnimationSpeed(speed);
    }
};

#endif // PLAYER_H_
