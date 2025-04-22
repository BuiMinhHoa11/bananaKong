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
        int distX = abs(centerX - obsCenterX);
        int distY = abs(centerY - obsCenterY);

        if (distX > (collisionBox.w / 2 + radius)) continue;
        if (distY > (collisionBox.h / 2 + radius)) continue;

        if (distX <= (collisionBox.w / 2)) return true;
        if (distY <= (collisionBox.h / 2)) return true;

        int cornerDistSq = pow(distX - collisionBox.w / 2, 2) +
                          pow(distY - collisionBox.h / 2, 2);

        if (cornerDistSq <= pow(radius, 2)) return true;
    }
    return false;
}

SDL_Rect ObstacleManager::adjustCollisionBox(const SDL_Rect& obsBox, ObstacleType type) const {
    SDL_Rect adjusted = obsBox;
    switch (type) {
        case ObstacleType::ROCK:
            adjusted.x += 10;
            adjusted.w = 200;
            adjusted.y += 50;
            adjusted.h = 90;
            break;
        case ObstacleType::SPIKE:
            adjusted.x += 16;
            adjusted.w = 30;
            adjusted.y += 23;
            adjusted.h = 90;
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
