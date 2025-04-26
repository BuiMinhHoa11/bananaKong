#include "player.h"
#include "obstacle.h"
#include "platform.h"
#include "common_func.h"
#include <algorithm>

Player::Player() {
    x = 400;
    y = GROUND_LEVEL - KONG_HEIGHT; // Khởi tạo tại mặt đất: 888 - 150 = 738
    velocityX = 0;
    velocityY = 0;
    gravity = 2000.0f;
    jumpForce = -800.0f;
    width = 200;
    height = KONG_HEIGHT; // 150
    onGround = true;
    isClimbingDown = false;
    state = RUNNING;
    collisionRadius = 50;
    showCollision = true;
}

Player::~Player() {}

void Player::init(SDL_Texture* runTexture) {
    runSprite.texture = runTexture;

    for (int i = 0; i < KONGRUN_FRAMES; i++) {
        SDL_Rect clip;
        clip.x = KONGRUN_CLIPS[i][0];
        clip.y = KONGRUN_CLIPS[i][1];
        clip.w = KONGRUN_CLIPS[i][2];
        clip.h = KONGRUN_CLIPS[i][3];
        runSprite.clips.push_back(clip);
    }
    runSprite.frameDelayMax = 4;
}

void Player::update(float deltaTime, const std::vector<SDL_Rect>& platforms, PlatformManager& platformManager) {
    // Kiểm tra va chạm với platform
    bool onAnyGround = false;
    for (const auto& platform : platforms) {
        if (checkPlatformCollision(platform)) {
            onAnyGround = true;
            break;
        }
    }

    // Cập nhật trạng thái onGround
    if (!onAnyGround && y < GROUND_LEVEL - height - 5) {
        onGround = false; // Không trên platform và chưa chạm mặt đất
    }

    // Áp dụng trọng lực nếu không trên mặt đất hoặc platform
    if (!onGround) {
        velocityY += gravity * deltaTime;
        y += velocityY * deltaTime;
    }

    x = 400;

    // Đặt nhân vật về mặt đất thực tế nếu vượt quá GROUND_LEVEL
    if (!onAnyGround && y > GROUND_LEVEL - height) {
        y = GROUND_LEVEL - height; // Đáy sprite chạm y = 888
        velocityY = 0;
        onGround = true;
    }

    if (onGround) {
        state = RUNNING;
        isClimbingDown = false;
    } else {
        if (velocityY < 0) {
            state = JUMPING;
        } else {
            state = FALLING;
        }
    }

    runSprite.tick();
}

void Player::climbDown(PlatformManager& platformManager) {
    if (!onGround || isClimbingDown) {
        return; // Không leo xuống nếu không trên platform hoặc đang leo xuống
    }

    const auto& platforms = platformManager.getPlatforms();
    float currentY = y;
    float targetY = GROUND_LEVEL; // Mặc định là mặt đất: y = 888
    bool foundPlatform = false;

    // Tìm platform gần nhất phía dưới nhân vật
    for (const auto& platform : platforms) {
        if (!platform.active) continue;
        float platformY = platform.rect.y;
        // Kiểm tra platform thấp hơn đáy nhân vật
        if (platformY > currentY + height) {
            // Đảm bảo nhân vật có thể đứng trong phạm vi ngang của platform
            if (x + width >= platform.rect.x && x <= platform.rect.x + platform.rect.w) {
                if (!foundPlatform || platformY < targetY) {
                    targetY = platformY;
                    foundPlatform = true;
                }
            }
        }
    }

    // Di chuyển xuống platform hoặc mặt đất
    velocityY = 300.0f; // Tốc độ rơi kiểm soát
    isClimbingDown = true;
    onGround = false;
    state = FALLING;

    // Điều chỉnh vị trí y để đứng trên bề mặt mục tiêu
    y = targetY - height; // Đáy sprite chạm targetY
}

void Player::render(Graphics* graphics) {
    graphics->render(x, y, runSprite);
    if (showCollision) {
        renderDebugCollision(graphics);
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

void Player::jump() {
    if (onGround) {
        velocityY = jumpForce;
        onGround = false;
        state = JUMPING;
    }
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

bool Player::checkPlatformCollision(const SDL_Rect& obstacle) {
    SDL_Point playerCenter = getCollisionCenter();
    int playerRadius = getCollisionRadius();
    if (velocityY >= 0) {
        int closestX = std::max(obstacle.x, std::min(playerCenter.x, obstacle.x + obstacle.w));
        int closestY = std::max(obstacle.y, std::min(playerCenter.y, obstacle.y + obstacle.h));
        int deltaX = playerCenter.x - closestX;
        int deltaY = playerCenter.y - closestY;
        int distanceSquared = (deltaX * deltaX) + (deltaY * deltaY);
        if (distanceSquared <= (playerRadius * playerRadius)) {
            if (closestY <= obstacle.y + 15) {
                y = obstacle.y - height + (playerCenter.y - y - playerRadius); // Căn chỉnh mượt mà
                velocityY = 0;
                setOnGround(true);
                return true;
            }
        }
    }
    return false;
}

void Player::setOnGround(bool grounded) {
    onGround = grounded;
    if (grounded && (state == JUMPING || state == FALLING)) {
        state = RUNNING;
    }
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
