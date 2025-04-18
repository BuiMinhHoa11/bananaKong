/*#include "obstacle.h"
#include "common_func.h"
#include "graphics.h"
#include "player.h"
#include <algorithm>
#include <cstdlib>

ObstacleManager::ObstacleManager(const std::map<ObstacleType, SDL_Texture*>& textures)
    : textureMap(textures), timeSinceLastSpawn(0.0f),
      baseSpawnInterval(3.0f), minSpawnInterval(1.5f), difficulty(0.0f) {
    srand(static_cast<unsigned int>(time(nullptr)));
}

ObstacleManager::~ObstacleManager() {
    obstacles.clear();
}

void ObstacleManager::update(float deltaTime) {
    timeSinceLastSpawn += deltaTime;

    if (timeSinceLastSpawn >= getRandomSpawnInterval()) {
        spawn();
        timeSinceLastSpawn = 0.0f;
    }

    float baseSpeed = 10.0f + difficulty * 10.0f;

    for (auto it = obstacles.begin(); it != obstacles.end();) {
        it->move(baseSpeed);
        if (it->isOffScreen()) {
            it = obstacles.erase(it);
        } else {
            ++it;
        }
    }

    difficulty = std::min(difficulty + deltaTime * 0.002f, 1.0f);
}

float ObstacleManager::getRandomSpawnInterval() const {
    float interval = baseSpawnInterval - (baseSpawnInterval - minSpawnInterval) * difficulty;
    float randomFactor = (static_cast<float>(rand()) / RAND_MAX) * 1.0f - 0.5f;
    return std::max(interval + randomFactor, minSpawnInterval);
}

ObstacleType ObstacleManager::getRandomObstacleType() const {
    return static_cast<ObstacleType>(rand() % 3);
}

float ObstacleManager::getRandomSpeed() const {
    return 1.0f + (static_cast<float>(rand()) / RAND_MAX) * 0.2f - 0.1f;
}

int ObstacleManager::getNumberToSpawn() const {
    float maxObstacles = 1.0f + difficulty * 2.0f;
    return std::min(1 + rand() % static_cast<int>(maxObstacles), 3);
}

int ObstacleManager::getRandomXOffset(int index) const {
    int baseOffset = 300;
    int randomVariation = rand() % 100 - 50;
    return index * (baseOffset + randomVariation);
}

void ObstacleManager::spawn() {
    int numToSpawn = getNumberToSpawn();
    for (int i = 0; i < numToSpawn; ++i) {
        Obstacle obs;
        ObstacleType type = getRandomObstacleType();
        obs.type = type;
        obs.texture = textureMap.at(type);
        obs.speed = getRandomSpeed();
        obs.isPlatform = true;

        obs.rect.x = SCREEN_WIDTH + getRandomXOffset(i);

        switch (type) {
            case ObstacleType::ROCK:
                obs.rect.w = 209;
                obs.rect.h = 133;
                obs.rect.y = 750;
                break;

            case ObstacleType::SPIKE:
                obs.rect.w = 106;
                obs.rect.h = 133;
                obs.rect.y = 750;
                break;

            case ObstacleType::HOUSE:
                obs.rect.w = 270;
                obs.rect.h = 133;
                obs.rect.y = 750;
                break;
        }

        obstacles.push_back(obs);
    }
}

SDL_Rect ObstacleManager::adjustCollisionBox(const SDL_Rect& obsBox, ObstacleType type) const {
    SDL_Rect adjustedBox = obsBox;

    switch (type) {
        case ObstacleType::ROCK:
            adjustedBox.x += static_cast<int>(adjustedBox.w * 0.15);
            adjustedBox.y += static_cast<int>(adjustedBox.h * 0.1);
            adjustedBox.w -= static_cast<int>(adjustedBox.w * 0.3);
            adjustedBox.h -= static_cast<int>(adjustedBox.h * 0.2);
            break;

        case ObstacleType::SPIKE:
            adjustedBox.x += static_cast<int>(adjustedBox.w * 0.25);
            adjustedBox.y += static_cast<int>(adjustedBox.h * 0.3);
            adjustedBox.w -= static_cast<int>(adjustedBox.w * 0.5);
            adjustedBox.h -= static_cast<int>(adjustedBox.h * 0.5);
            break;

        case ObstacleType::HOUSE:
            adjustedBox.x += 35;
            adjustedBox.w = 200;
            //y không đổi vì chiều cao giữ nguyên
            adjustedBox.h = 130;
            break;
    }

    return adjustedBox;
}

void ObstacleManager::renderDebugCollision(Graphics* graphics) const {
    for (const auto& obs : obstacles) {
        SDL_Rect obsBox = adjustCollisionBox(obs.rect, obs.type);
        SDL_SetRenderDrawColor(graphics->getRenderer(), 0, 255, 0, 128);
        SDL_RenderDrawRect(graphics->getRenderer(), &obsBox);
    }
}

bool ObstacleManager::checkCollision(const SDL_Rect& playerBox) const {
    for (const auto& obs : obstacles) {
        SDL_Rect obsBox = adjustCollisionBox(obs.rect, obs.type);
        if (SDL_HasIntersection(&playerBox, &obsBox)) {
            return true;
        }
    }
    return false;
}
*/
#include "obstacle.h"
#include "common_func.h"
#include "graphics.h"
#include "player.h"
#include <algorithm>
#include <cstdlib>
#include <cmath> // Thêm để sử dụng hàm sqrt

ObstacleManager::ObstacleManager(const std::map<ObstacleType, SDL_Texture*>& textures)
    : textureMap(textures), timeSinceLastSpawn(0.0f),
      baseSpawnInterval(3.0f), minSpawnInterval(1.5f), difficulty(0.0f) {
    srand(static_cast<unsigned int>(time(nullptr)));
}

ObstacleManager::~ObstacleManager() {
    obstacles.clear();
}

void ObstacleManager::update(float deltaTime) {
    timeSinceLastSpawn += deltaTime;

    if (timeSinceLastSpawn >= getRandomSpawnInterval()) {
        spawn();
        timeSinceLastSpawn = 0.0f;
    }

    float baseSpeed = 10.0f + difficulty * 10.0f;

    for (auto it = obstacles.begin(); it != obstacles.end();) {
        it->move(baseSpeed);
        if (it->isOffScreen()) {
            it = obstacles.erase(it);
        } else {
            ++it;
        }
    }

    difficulty = std::min(difficulty + deltaTime * 0.002f, 1.0f);
}

float ObstacleManager::getRandomSpawnInterval() const {
    float interval = baseSpawnInterval - (baseSpawnInterval - minSpawnInterval) * difficulty;
    float randomFactor = (static_cast<float>(rand()) / RAND_MAX) * 1.0f - 0.5f;
    return std::max(interval + randomFactor, minSpawnInterval);
}

ObstacleType ObstacleManager::getRandomObstacleType() const {
    return static_cast<ObstacleType>(rand() % 3);
}

float ObstacleManager::getRandomSpeed() const {
    return 1.0f + (static_cast<float>(rand()) / RAND_MAX) * 0.2f - 0.1f;
}

int ObstacleManager::getNumberToSpawn() const {
    float maxObstacles = 1.0f + difficulty * 2.0f;
    return std::min(1 + rand() % static_cast<int>(maxObstacles), 3);
}

int ObstacleManager::getRandomXOffset(int index) const {
    int baseOffset = 300;
    int randomVariation = rand() % 100 - 50;
    return index * (baseOffset + randomVariation);
}

void ObstacleManager::spawn() {
    int numToSpawn = getNumberToSpawn();
    for (int i = 0; i < numToSpawn; ++i) {
        Obstacle obs;
        ObstacleType type = getRandomObstacleType();
        obs.type = type;
        obs.texture = textureMap.at(type);
        obs.speed = getRandomSpeed();
        obs.isPlatform = true;

        obs.rect.x = SCREEN_WIDTH + getRandomXOffset(i);

        switch (type) {
            case ObstacleType::ROCK:
                obs.rect.w = 209;
                obs.rect.h = 133;
                obs.rect.y = 755;
                break;

            case ObstacleType::SPIKE:
                obs.rect.w = 106;
                obs.rect.h = 133;
                obs.rect.y = 750;
                break;

            case ObstacleType::PLANE:
                obs.rect.w = 270;
                obs.rect.h = 133;
                obs.rect.y = 755;
                break;
        }

        obstacles.push_back(obs);
    }
}

SDL_Rect ObstacleManager::adjustCollisionBox(const SDL_Rect& obsBox, ObstacleType type) const {
    SDL_Rect adjustedBox = obsBox;

    switch (type) {
        case ObstacleType::ROCK:
            //adjustedBox.x += 25;
            //adjustedBox.w = 159; // 209 - mỗi bên 25= 209-25-25
            adjustedBox.y += 50;
            adjustedBox.h = 103;
            break;

        case ObstacleType::SPIKE:
            adjustedBox.x += 10;
            adjustedBox.w = 60;
            adjustedBox.y += 23;
            adjustedBox.h = 90;
            break;

        case ObstacleType::PLANE:
            adjustedBox.x += 25;
            adjustedBox.w = 230;
            adjustedBox.y += 30;
            adjustedBox.h = 100;
            break;
    }

    return adjustedBox;
}

void ObstacleManager::renderDebugCollision(Graphics* graphics) const {
    for (const auto& obs : obstacles) {
        SDL_Rect obsBox = adjustCollisionBox(obs.rect, obs.type);

        // Vẽ nền mờ để dễ nhìn hơn
        SDL_SetRenderDrawColor(graphics->getRenderer(), 255, 0, 0, 60);
        SDL_SetRenderDrawBlendMode(graphics->getRenderer(), SDL_BLENDMODE_BLEND);
        SDL_RenderFillRect(graphics->getRenderer(), &obsBox);

        // Vẽ viền xanh lá
        SDL_SetRenderDrawColor(graphics->getRenderer(), 0, 255, 0, 255);
        SDL_RenderDrawRect(graphics->getRenderer(), &obsBox);
    }
}

// Hàm mới để kiểm tra va chạm giữa hình tròn và hình chữ nhật
bool ObstacleManager::checkCollision(int centerX, int centerY, int radius) const {
    for (const auto& obs : obstacles) {
        SDL_Rect obsBox = adjustCollisionBox(obs.rect, obs.type);

        // Tìm điểm gần nhất của hình chữ nhật với tâm hình tròn
        int closestX = std::max(obsBox.x, std::min(centerX, obsBox.x + obsBox.w));
        int closestY = std::max(obsBox.y, std::min(centerY, obsBox.y + obsBox.h));

        // Tính khoảng cách từ điểm gần nhất đến tâm hình tròn
        int distanceX = centerX - closestX;
        int distanceY = centerY - closestY;
        int distanceSquared = distanceX * distanceX + distanceY * distanceY;

        // Nếu khoảng cách nhỏ hơn bán kính, có va chạm
        if (distanceSquared <= radius * radius) {
            return true;
        }
    }
    return false;
}
