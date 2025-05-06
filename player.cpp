#include "player.h"
#include "obstacle.h"
#include "platform.h"
#include "common_func.h"
#include <algorithm>

Player::Player(AudioManager& audioManager) : audioManager(audioManager) {
    x = 400;
    y = KONG_DRAW_Y_START;
    velocityX = 0;
    velocityY = 0;
    gravity = 2000.0f;
    jumpForce = -916.0f;
    width = 200;
    height = KONG_HEIGHT;
    onGround = true;
    isClimbingDown = false;
    state = RUN;
    collisionRadius = 50;
    showCollision = true;
    flyStartY = 0.0f;
}

Player::~Player() {}

void Player::init(SDL_Texture* runTexture, SDL_Texture* flyTexture, SDL_Texture* dieTexture) {
    runSprite.init(runTexture, KONGRUN_FRAMES, KONGRUN_CLIPS);
    runSprite.frameDelayMax = 3;

    flySprite.init(flyTexture, KONGFLY_FRAMES, KONGFLY_CLIPS);
    flySprite.frameDelayMax = 0;
    flySprite.currentFrame = 0;

    dieSprite.init(dieTexture, KONGDIE_FRAMES, KONGDIE_CLIPS);
    dieSprite.frameDelayMax = 0;
    dieSprite.currentFrame = 0;
}

void Player::update(float deltaTime, const std::vector<SDL_Rect>& platforms, PlatformManager& platformManager) {
    if (state == DIE) {
        return;
    }

    bool onAnyGround = false;
    for (const auto& platform : platforms) {
        if (checkPlatformCollision(platform)) {
            onAnyGround = true;
            break;
        }
    }

    if (!onAnyGround && y < KONG_DRAW_Y_START - 5) {
        onGround = false;
    }

    if (!onGround) {
        if (state == FLY) {
            if (y < flyStartY - 210.0f) {
                y = flyStartY - 210.0f;
                velocityY = 100.0f;
            } else if (velocityY < 0) {
                velocityY += gravity * deltaTime * 0.5f;
                y += velocityY * deltaTime;
            } else {
                velocityY = 150.0f;
                y += velocityY * deltaTime;
            }
        } else {
            velocityY += gravity * deltaTime;
            y += velocityY * deltaTime;
        }
    }

    x = 400;

    if (!onAnyGround && y > KONG_DRAW_Y_START) {
        y = KONG_DRAW_Y_START;
        velocityY = 0;
        onGround = true;
        if (state == FLY) {
            state = RUN;
        }
    }

    if (onGround) {
        if (state == RUN && velocityX == 0 && velocityY == 0) {
            state = IDLE;
            // audioManager.playSound(SoundType::IDLE); // Đã comment vì chưa cần dùng
        } else if (state != IDLE) {
            state = RUN;
        }
        isClimbingDown = false;
    } else {
        if (state != FLY) {
            if (velocityY < 0) {
                state = JUMP;
            } else {
                state = FALL;
            }
        }
    }

    runSprite.tick();
}

void Player::climbDown(PlatformManager& platformManager) {
    if (!onGround || isClimbingDown) {
        return;
    }

    const auto& platforms = platformManager.getPlatforms();
    float currentY = y;
    float targetY = GROUND_LEVEL;
    bool foundPlatform = false;

    for (const auto& platform : platforms) {
        if (!platform.active) continue;
        float platformY = platform.rect.y;
        if (platformY > currentY + height) {
            if (x + width >= platform.rect.x && x <= platform.rect.x + platform.rect.w) {
                if (!foundPlatform || platformY < targetY) {
                    targetY = platformY;
                    foundPlatform = true;
                }
            }
        }
    }

    velocityY = 300.0f;
    isClimbingDown = true;
    onGround = false;
    state = FALL;
    y = targetY - height;
    audioManager.playSound(SoundType::CLIMB_DOWN);
}

void Player::render(Graphics* graphics) {
    if (state == FLY) {
        graphics->render(x, y, flySprite);
    } else if (state == DIE) {
        graphics->render(x, y + 15, dieSprite);
    } else {
        graphics->render(x, y, runSprite);
    }
    if (showCollision) {
        renderDebugCollision(graphics);
    }
}

void Player::startFly() {
    if (state == JUMP || state == FALL) {
        state = FLY;
        flyStartY = y;
        if (velocityY < 0) {
            velocityY *= 0.44f;
        } else {
            velocityY = -200.0f;
        }
    }
}

void Player::stopFly() {
    if (state == FLY) {
        state = FALL;
        velocityY = 100.0f;
    }
}

void Player::jump() {
    if (onGround) {
        velocityY = jumpForce;
        onGround = false;
        state = JUMP;
        audioManager.playSound(SoundType::JUMP);
    }
}

bool Player::checkPlatformCollision(const SDL_Rect& obstacle) {
    SDL_Point playerCenter = getCollisionCenter();
    int playerRadius = getCollisionRadius();
    if (velocityY >= 0 || state == FLY) {
        int closestX = std::max(obstacle.x, std::min(playerCenter.x, obstacle.x + obstacle.w));
        int closestY = std::max(obstacle.y, std::min(playerCenter.y, obstacle.y + obstacle.h));
        int deltaX = playerCenter.x - closestX;
        int deltaY = playerCenter.y - closestY;
        int distanceSquared = (deltaX * deltaX) + (deltaY * deltaY);
        if (distanceSquared <= (playerRadius * playerRadius)) {
            if (closestY <= obstacle.y + 15) {
                y = obstacle.y - height + (playerCenter.y - y - playerRadius);
                velocityY = 0;
                setOnGround(true);
                if (state == FLY) {
                    state = RUN;
                }
                return true;
            }
        }
    }
    return false;
}

void Player::setOnGround(bool grounded) {
    onGround = grounded;
    if (grounded && (state == JUMP || state == FALL || state == FLY)) {
        state = RUN;
    }
}

void Player::renderDebugCollision(Graphics* graphics) {
    renderCircularCollision(graphics);
}

void Player::renderCircularCollision(Graphics* graphics) {
    SDL_Point center = getCollisionCenter();
    int radius = getCollisionRadius();

    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(graphics->getRenderer(), &r, &g, &b, &a);
    SDL_SetRenderDrawColor(graphics->getRenderer(), 255, 0, 0, 255);

    int x = radius;
    int y = 0;
    int err = 0;
    while (x >= y) {
        SDL_RenderDrawPoint(graphics->getRenderer(), center.x + x, center.y + y);
        SDL_RenderDrawPoint(graphics->getRenderer(), center.x + y, center.y + x);
        SDL_RenderDrawPoint(graphics->getRenderer(), center.x - y, center.y + x);
        SDL_RenderDrawPoint(graphics->getRenderer(), center.x - x, center.y + y);
        SDL_RenderDrawPoint(graphics->getRenderer(), center.x - x, center.y - y);
        SDL_RenderDrawPoint(graphics->getRenderer(), center.x - y, center.y - x);
        SDL_RenderDrawPoint(graphics->getRenderer(), center.x + y, center.y - x);
        SDL_RenderDrawPoint(graphics->getRenderer(), center.x + x, center.y - y);

        if (err <= 0) { y += 1; err += 2*y + 1; }
        if (err > 0) { x -= 1; err -= 2*x + 1; }
    }

    SDL_SetRenderDrawColor(graphics->getRenderer(), 0, 255, 0, 255);
    for (int i = -2; i <= 2; i++) {
        for (int j = -2; j <= 2; j++) {
            SDL_RenderDrawPoint(graphics->getRenderer(), center.x + i, center.y + j);
        }
    }

    SDL_SetRenderDrawColor(graphics->getRenderer(), 0, 255, 0, 128);
    SDL_RenderDrawLine(graphics->getRenderer(), center.x - radius, center.y - radius, center.x + radius, center.y + radius);
    SDL_RenderDrawLine(graphics->getRenderer(), center.x - radius, center.y + radius, center.x + radius, center.y - radius);
    SDL_SetRenderDrawColor(graphics->getRenderer(), r, g, b, a);
}

SDL_Point Player::getCollisionCenter() const {
    SDL_Point center;
    center.x = static_cast<int>(x + 57);
    center.y = static_cast<int>(y + 82);
    return center;
}

int Player::getCollisionRadius() const {
    return collisionRadius;
}

bool Player::checkCircularCollision(const SDL_Point& otherCenter, int otherRadius) const {
    SDL_Point myCenter = getCollisionCenter();
    int myRadius = getCollisionRadius();
    int deltaX = myCenter.x - otherCenter.x;
    int deltaY = myCenter.y - otherCenter.y;
    int distanceSquared = (deltaX * deltaX) + (deltaY * deltaY);
    int radiusSum = myRadius + otherRadius;
    return distanceSquared <= (radiusSum * radiusSum);
}

SDL_Rect Player::getCollisionBox() const {
    SDL_Rect box;
    int paddingX = static_cast<int>(width * 0.35);
    int paddingY = static_cast<int>(height * 0.2);
    box.x = static_cast<int>(x) + paddingX;
    box.y = static_cast<int>(y) + paddingY;
    box.w = static_cast<int>(width) - (paddingX * 2);
    box.h = static_cast<int>(height) - (paddingY * 2);
    return box;
}

void Player::setPosition(float newX, float newY) {
    x = newX;
    y = newY;
}

void Player::setCollisionRadius(int radius) {
    collisionRadius = radius;
}

void Player::toggleCollisionDisplay() {
    showCollision = !showCollision;
}

void Player::setState(PlayerState newState) {
    state = newState;
}
