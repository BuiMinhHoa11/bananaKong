#include "obstacle.h"
#include <cstdlib>

ObstacleManager::ObstacleManager(const std::map<ObstacleType, SDL_Texture*>& textures) {
    textureMap = textures;
    difficulty = 1.0f;
}

ObstacleManager::~ObstacleManager() {
    obstacles.clear();
}

void ObstacleManager::update(float deltaTime) {
    for (auto it = obstacles.begin(); it != obstacles.end();) {
        it->rect.x -= static_cast<int>(5.0f * it->speed);
        if (it->rect.x + it->rect.w < 0) {
            it = obstacles.erase(it);
        } else {
            ++it;
        }
    }
}

bool ObstacleManager::checkCollision(int centerX, int centerY, int radius) const {
    for (const auto& obstacle : obstacles) {
        SDL_Rect collisionBox = adjustCollisionBox(obstacle.rect, obstacle.type);
        int obsCenterX = collisionBox.x + collisionBox.w / 2;
        int obsCenterY = collisionBox.y + collisionBox.h / 2;
        int distX = centerX - obsCenterX;
        int distY = centerY - obsCenterY;

        // Kiểm tra nếu Kong ở phía trên chướng ngại vật (cho phép đứng lên)
        if (obstacle.isPlatform) {
            // Tính toán vị trí chân của Kong
            int kongBottomY = centerY + radius;
            int obstacleTopY = collisionBox.y;

            // Nếu chân của Kong ở trên đỉnh chướng ngại vật và không va chạm từ các phía khác
            if (kongBottomY >= obstacleTopY && kongBottomY <= obstacleTopY + 10 && // Chân của Kong gần đỉnh chướng ngại vật
                abs(distX) <= (collisionBox.w / 2 + radius) && // Trong phạm vi chiều ngang của chướng ngại vật
                distY < 0) { // Kong ở phía trên
                continue; // Không gây GAME_OVER, cho phép đứng lên
            }
        }

        // Kiểm tra va chạm thông thường (gây GAME_OVER nếu va chạm từ các phía khác)
        int absDistX = abs(distX);
        int absDistY = abs(distY);

        if (absDistX > (collisionBox.w / 2 + radius)) continue;
        if (absDistY > (collisionBox.h / 2 + radius)) continue;

        if (absDistX <= (collisionBox.w / 2)) return true;
        if (absDistY <= (collisionBox.h / 2)) return true;

        int cornerDistSq = pow(absDistX - collisionBox.w / 2, 2) +
                          pow(absDistY - collisionBox.h / 2, 2);

        if (cornerDistSq <= pow(radius, 2)) return true;
    }
    return false;
}

SDL_Rect ObstacleManager::adjustCollisionBox(const SDL_Rect& obsBox, ObstacleType type) const {
    SDL_Rect adjusted = obsBox;
    switch (type) {
        case ObstacleType::ROCK:
            adjusted.x += 10;
            adjusted.w = 180;
            adjusted.y += 50;
            adjusted.h = 80;
            break;
        case ObstacleType::SPIKE:
            adjusted.x += 16;
            adjusted.w = 30;
            adjusted.y += 23;
            adjusted.h = 100;
            break;
        case ObstacleType::PLANE:
            adjusted.x += 25;
            adjusted.w = 220;
            adjusted.y += 30;
            adjusted.h = 90;
            break;
    }
    return adjusted;
}

void ObstacleManager::renderDebugCollision(Graphics* graphics) const {
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(graphics->getRenderer(), &r, &g, &b, &a);
    SDL_SetRenderDrawColor(graphics->getRenderer(), 255, 0, 0, 128);

    for (const auto& obstacle : obstacles) {
        SDL_Rect collisionBox = adjustCollisionBox(obstacle.rect, obstacle.type);
        SDL_RenderDrawRect(graphics->getRenderer(), &collisionBox);
    }

    SDL_SetRenderDrawColor(graphics->getRenderer(), r, g, b, a);
}

ObstacleType ObstacleManager::getRandomObstacleType() const {
    int r = rand() % 3;
    switch (r) {
        case 0: return ObstacleType::ROCK;
        case 1: return ObstacleType::SPIKE;
        default: return ObstacleType::PLANE;
    }
}

float ObstacleManager::getRandomSpeed() const {
    return 0.8f + (static_cast<float>(rand()) / RAND_MAX) * 0.4f;
}
