#include "obstacle.h"
#include <cstdlib>

ObstacleManager::ObstacleManager(const std::map<ObstacleType, SDL_Texture*>& textures) {
    textureMap = textures;
    difficulty = 1.0f;
}

ObstacleManager::~ObstacleManager() {
    obstacles.clear();
}

void ObstacleManager::update(float deltaTime, float scrollSpeed) {
    for (auto it = obstacles.begin(); it != obstacles.end();) {
        it->rect.x -= static_cast<int>(scrollSpeed);
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
            int kongBottomY = centerY + radius;
            int obstacleTopY = collisionBox.y;

            if (kongBottomY >= obstacleTopY && kongBottomY <= obstacleTopY + 10 &&
                abs(distX) <= (collisionBox.w / 2 + radius) &&
                distY < 0) {
                continue;
            }
        }

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
            adjusted.w = 170;
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
            adjusted.x += 32;
            adjusted.w = 200;
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

void ObstacleManager::addObstacle(const Obstacle& obstacle) {
    obstacles.push_back(obstacle);
}

SDL_Texture* ObstacleManager::getTextureForType(ObstacleType type) const {
    auto it = textureMap.find(type);
    if (it != textureMap.end()) {
        return it->second;
    }
    return nullptr;
}

void ObstacleManager::setDifficulty(float diff) {
    difficulty = diff;
}

const std::vector<Obstacle>& ObstacleManager::getObstacles() const {
    return obstacles;
}

void ObstacleManager::clear() {
    obstacles.clear();
}
