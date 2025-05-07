#ifndef PLAYER_H_
#define PLAYER_H_
#include "common_func.h"
#include "graphics.h"
#include "audio.h"

class PlatformManager;

enum PlayerState {
    IDLE,
    RUN,
    JUMP,
    FALL,
    FLY,
    DIE
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
    Sprite flySprite;
    Sprite dieSprite;
    int collisionRadius;
    bool showCollision;
    float flyStartY;
    AudioManager& audioManager;

    bool checkPlatformCollision(const SDL_Rect& obstacle);

public:
    Player(AudioManager& audioManager);
    ~Player();

    void init(SDL_Texture* runTexture, SDL_Texture* flyTexture, SDL_Texture* dieTexture);
    void update(float deltaTime, const std::vector<SDL_Rect>& platforms, PlatformManager& platformManager);
    void render(Graphics* graphics);
    void renderDebugCollision(Graphics* graphics);
    void renderCircularCollision(Graphics* graphics);

    void jump();
    void climbDown(PlatformManager& platformManager);
    void startFly();
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
    void setState(PlayerState newState);

    void toggleCollisionDisplay();
    bool isCollisionDisplayed() const { return showCollision; }

    void setAnimationSpeed(int speed) {
        runSprite.setAnimationSpeed(speed);
    }
};

#endif // PLAYER_H_
