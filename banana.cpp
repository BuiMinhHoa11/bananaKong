#include "banana.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <set>

Banana::Banana(SDL_Texture* tex, BananaType t, int x, int y) {
    texture = tex;
    type = t;
    active = true;
    rect.w = 40;
    rect.h = 40;
    value = 1;
    rect.x = x;
    rect.y = y;
}

BananaManager::BananaManager(std::map<BananaType, SDL_Texture*> textures, const PlatformManager& platformMgr, const ObstacleManager& obstacleMgr)
    : platformManager(platformMgr), obstacleManager(obstacleMgr) {
    bananaTextures = textures;
    scrollSpeed = 4.0f;
    spawnTimer = 0.0f;
    spawnInterval = 2.0f;
    difficultyFactor = 1.0f;
    srand(static_cast<unsigned>(time(nullptr)));
    lastSpawnTime = 0.0f;
}

BananaManager::~BananaManager() {
    bananas.clear();
    spawnedPlatforms.clear();
    collectedPositions.clear();
}

void BananaManager::update(float deltaTime) {
    spawnTimer += deltaTime;

    // Update banana positions with scrolling
    for (auto it = bananas.begin(); it != bananas.end();) {
        it->rect.x -= static_cast<int>(scrollSpeed);
        if (!it->active || it->rect.x + it->rect.w < -100) {
            it = bananas.erase(it);
        } else {
            ++it;
        }
    }

    // Clean up spawnedPlatforms
    for (auto it = spawnedPlatforms.begin(); it != spawnedPlatforms.end();) {
        bool platformExists = false;
        for (const auto& platform : platformManager.getPlatforms()) {
            if (platform.rect.x == *it && platform.active && platform.rect.x + platform.rect.w > -100) {
                platformExists = true;
                break;
            }
        }
        if (!platformExists) {
            it = spawnedPlatforms.erase(it);
        } else {
            ++it;
        }
    }

    // Clean up collectedPositions
    collectedPositions.erase(
        std::remove_if(collectedPositions.begin(), collectedPositions.end(),
            [](const SDL_Point& pos) { return pos.x < -100; }),
        collectedPositions.end()
    );

    // Spawn bananas when timer exceeds interval
    if (spawnTimer - lastSpawnTime >= spawnInterval) {
        spawnBananas();
        lastSpawnTime = spawnTimer;
    }
}

void BananaManager::render(SDL_Renderer* renderer) {
    for (const auto& banana : bananas) {
        if (banana.active && banana.rect.x >= -40 && banana.rect.x <= SCREEN_WIDTH) {
            SDL_RenderCopyEx(renderer, banana.texture, NULL, &banana.rect, 0, NULL, SDL_FLIP_NONE);
        }
    }
}

bool BananaManager::checkCollision(int playerX, int playerY, int playerRadius, int& score, bool& magnetActivated) {
    bool collected = false;
    magnetActivated = false;

    for (auto it = bananas.begin(); it != bananas.end();) {
        if (!it->active) {
            ++it;
            continue;
        }

        int bananaX = it->rect.x + it->rect.w / 2;
        int bananaY = it->rect.y + it->rect.h / 2;
        int distX = playerX - bananaX;
        int distY = playerY - bananaY;
        float distance = sqrt(distX * distX + distY * distY);

        if (distance <= (playerRadius + it->rect.w / 2)) {
            score += it->value;
            collected = true;

            // Store collected banana position
            SDL_Point pos = {bananaX, bananaY};
            collectedPositions.push_back(pos);

            it->active = false;
            it = bananas.erase(it);
        } else {
            ++it;
        }
    }

    return collected;
}

void BananaManager::spawnBananas() {
    std::vector<Platform> sortedPlatforms;
    const auto& platforms = platformManager.getPlatforms();

    // Find furthest platform edge
    int furthestX = SCREEN_WIDTH;
    for (const auto& platform : platforms) {
        if (platform.active) {
            int platformRight = platform.rect.x + platform.rect.w;
            if (platformRight > furthestX) {
                furthestX = platformRight;
            }
            sortedPlatforms.push_back(platform);
        }
    }
    std::sort(sortedPlatforms.begin(), sortedPlatforms.end(),
              [](const Platform& a, const Platform& b) { return a.rect.x < b.rect.x; });

    // Spawn bananas on platforms beyond SCREEN_WIDTH
    for (const auto& platform : sortedPlatforms) {
        if (platform.rect.x >= SCREEN_WIDTH - 700 && spawnedPlatforms.find(platform.rect.x) == spawnedPlatforms.end()) {
            spawnOnPlatform(platform);
            spawnedPlatforms.insert(platform.rect.x);
        }
    }

    // Spawn bananas on ground beyond furthest platform
    int spawnStart = furthestX;
    int spawnEnd = spawnStart + 1000; // default value if no next platform found

    // Tìm platform tiếp theo sau furthestX
    for (const auto& platform : sortedPlatforms) {
        if (platform.rect.x > spawnStart) {
            spawnEnd = platform.rect.x;
            break;
        }
    }

    // Chỉ spawn nếu vùng spawn còn trong tầm người chơi có thể nhìn thấy
    if (spawnStart < SCREEN_WIDTH + 1000) {
        spawnOnGround(spawnStart, spawnEnd);
    }

}

void BananaManager::spawnOnPlatform(const Platform& platform) {
    int spacing = 80;
    int bananaCount = platform.rect.w / spacing;
    if (bananaCount < 1) bananaCount = 1;
    if (bananaCount > 15) bananaCount = 15;

    for (int i = 0; i < bananaCount; ++i) {
        int posX = platform.rect.x + i * spacing;
        int posY = platform.rect.y - 60;

        if (isNearCollectedPosition(posX, posY)) {
            continue;
        }

        SDL_Rect bananaRect = {posX, posY, 28, 60};
        bool overlapsObstacle = false;
        for (const auto& obs : obstacleManager.getObstacles()) {
            SDL_Rect expandedObsRect = obs.rect;
            expandedObsRect.x -= 10;
            expandedObsRect.y -= 10;
            expandedObsRect.w += 20;
            expandedObsRect.h += 20;
            if (SDL_HasIntersection(&bananaRect, &expandedObsRect)) {
                overlapsObstacle = true;
                break;
            }
        }

        if (overlapsObstacle || !isValidBananaPosition(posX, posY, 28, 60)) {
            continue;
        }

        BananaType type = BananaType::NORMAL;
        if (bananaTextures.find(type) != bananaTextures.end()) {
            Banana newBanana(bananaTextures.at(type), type, posX, posY);
            bananas.push_back(newBanana);
        }
    }
}

void BananaManager::spawnOnGround(int startX, int endX) {
    int spacing = 80;
    int bananaCount = (endX - startX) / spacing;
    if (bananaCount < 1) bananaCount = 1;
    if (bananaCount > 15) bananaCount = 15;

    for (int i = 0; i < bananaCount; ++i) {
        int posX = startX + i * spacing;
        int posY = GROUND_LEVEL - 80;

        if (isNearCollectedPosition(posX, posY)) {
            continue;
        }

        SDL_Rect bananaRect = {posX, posY, 28, 60};
        bool overlapsObstacle = false;
        for (const auto& obs : obstacleManager.getObstacles()) {
            SDL_Rect expandedObsRect = obs.rect;
            expandedObsRect.x -= 10;
            expandedObsRect.y -= 10;
            expandedObsRect.w += 50;
            expandedObsRect.h += 50;
            if (SDL_HasIntersection(&bananaRect, &expandedObsRect)) {
                overlapsObstacle = true;
                break;
            }
        }

        if (overlapsObstacle || !isValidBananaPosition(posX, posY, 28, 60)) {
            continue;
        }

        BananaType type = BananaType::NORMAL;
        if (bananaTextures.find(type) != bananaTextures.end()) {
            Banana newBanana(bananaTextures.at(type), type, posX, posY);
            bananas.push_back(newBanana);
        }
    }
}

bool BananaManager::isValidBananaPosition(int x, int y, int width, int height) const {
    if (y < 0 || y + height > GROUND_LEVEL) {
        return false;
    }

    int thisCenterX = x + width / 2;
    int thisCenterY = y + height / 2;
    const int minDistance = 50;

    for (const auto& banana : bananas) {
        int otherCenterX = banana.rect.x + banana.rect.w / 2;
        int otherCenterY = banana.rect.y + banana.rect.h / 2;
        int distX = thisCenterX - otherCenterX;
        int distY = thisCenterY - otherCenterY;
        float distance = sqrt(distX * distX + distY * distY);
        if (distance < minDistance) {
            return false;
        }
    }

    return true;
}

bool BananaManager::isNearCollectedPosition(int x, int y) const {
    int thisCenterX = x + 20;
    int thisCenterY = y + 20;
    const int minDistance = 100;

    for (const auto& pos : collectedPositions) {
        if (pos.x < 0 || pos.x > SCREEN_WIDTH) continue;
        int distX = thisCenterX - pos.x;
        int distY = thisCenterY - pos.y;
        float distance = sqrt(distX * distX + distY * distY);
        if (distance < minDistance) {
            return true;
        }
    }
    return false;
}

void BananaManager::setScrollSpeed(float speed) {
    scrollSpeed = speed;
}

void BananaManager::setDifficultyFactor(float difficulty) {
    difficultyFactor = difficulty;
    spawnInterval = 2.0f - (difficulty - 1.0f) * 0.15f;
    if (spawnInterval < 0.5f) spawnInterval = 0.5f;
}

void BananaManager::clear() {
    bananas.clear();
    spawnedPlatforms.clear();
    collectedPositions.clear();
}

void BananaManager::renderDebugCollision(SDL_Renderer* renderer) {
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);

    for (const auto& banana : bananas) {
        if (banana.active && banana.rect.x >= -40 && banana.rect.x <= SCREEN_WIDTH) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 128);
            SDL_Rect outlineRect = banana.rect;
            SDL_RenderDrawRect(renderer, &outlineRect);
        }
    }

    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}
