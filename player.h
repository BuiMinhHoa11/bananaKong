#ifndef PLAYER_H_
#define PLAYER_H_
#include "common_func.h"
#include "graphics.h"

enum PlayerState {
    IDLE,
    RUNNING,
    JUMPING,
    FALLING,
    SLIDING
};

class Player {
private:
    float x, y;
    float velocityX, velocityY;
    float gravity;
    float jumpForce;
    int width, height;
    bool onGround;
    PlayerState state;
    Sprite sprite;

public:
    Player();
    ~Player();

    void init(SDL_Texture* texture);
    void update(float deltaTime);
    void render(Graphics* graphics);

    void jump();
    void slide();
    void stopSliding();

    SDL_Rect getCollisionBox() const;
    void setOnGround(bool grounded);
    bool isOnGround() const { return onGround; }

    float getX() const { return x; }
    float getY() const { return y; }
    void setPosition(float newX, float newY);

    PlayerState getState() const { return state; }
};

#endif // PLAYER_H_
