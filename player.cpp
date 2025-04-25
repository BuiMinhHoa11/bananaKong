#include "player.h"
#include "obstacle.h"

Player::Player() {
    x = 400;
    y = 755;
    velocityX = 0;
    velocityY = 0;
    gravity = 2000.0f;
    jumpForce = -800.0f;
    width = 200;
    height = 167;
    onGround = true;
    state = RUNNING;
    collisionRadius = 45;
    showCollision = true;
}

Player::~Player() {
}

void Player::init(SDL_Texture* runTexture) {
    runSprite.texture = runTexture; // Đổi tên từ sprite thành runSprite

    // Khởi tạo clip cho sprite chạy
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

void Player::update(float deltaTime, const std::vector<SDL_Rect>& platforms) {
    velocityY += gravity * deltaTime;
    y += velocityY * deltaTime;

    bool onAnyGround = false;
    for (const auto& platform : platforms) {
        if (checkPlatformCollision(platform)) {
            onAnyGround = true;
            break;
        }
    }

    x = 400;

    if (!onAnyGround && y > 755) {
        y = 755;
        velocityY = 0;
        onAnyGround = true;
    }

    onGround = onAnyGround;

    if (!onGround) {
        if (velocityY < 0) {
            state = JUMPING;
        } else {
            state = FALLING;
        }
    } else {
        state = RUNNING;
    }

    runSprite.tick(); // Đổi tên từ sprite thành runSprite
}

void Player::render(Graphics* graphics) {
    graphics->render(x, y, runSprite); // Đổi tên từ sprite thành runSprite

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
    return collisionRadius; // Xóa logic điều chỉnh bán kính cho SLIDING
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
    return box; // Xóa logic điều chỉnh hộp va chạm cho SLIDING
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
                y = obstacle.y - height + (playerCenter.y - y - playerRadius);
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
