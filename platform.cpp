
#include "platform.h"
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <algorithm>

Platform::Platform(SDL_Texture* tex, PlatformType t, int x, int y) {
    texture = tex;
    type = t;
    active = true;

    switch(type) {
        case PlatformType::GRASS_BIG:
            rect.w = 590;
            rect.h = 45;
            break;
        case PlatformType::GRASS_MID:
            rect.w = 813;
            rect.h = 45;
            break;
        case PlatformType::GRASS_SUPERBIG:
            rect.w = 964;
            rect.h = 45;
            break;
        case PlatformType::LAND_MID:
            rect.w = 867;
            rect.h = 155;
            break;
        case PlatformType::LAND_SMALL:
            rect.w = 750;
            rect.h = 159;
            break;
    }

    rect.x = x;
    rect.y = y;
}

PlatformManager::PlatformManager(std::map<PlatformType, SDL_Texture*> textures, ObstacleManager& obsManager)
    : obstacleManager(obsManager) {
    platformTextures = textures;
    scrollSpeed = 4.0f;
    difficulty = 1.0f;
    difficultyTimer = 0;
    spawnTimer = 0.0f;
    difficultyIncreaseInterval = 25000;
    srand(static_cast<unsigned>(time(nullptr)));
}

void PlatformManager::update(float deltaTime) {
    spawnTimer += deltaTime;

    for (auto& platform : platforms) {
        platform.rect.x -= static_cast<int>(scrollSpeed);
        if (platform.rect.x + platform.rect.w < 0) {
            platform.active = false;
        }
    }

    platforms.erase(
        std::remove_if(platforms.begin(), platforms.end(),
            [](const Platform& p) { return !p.active; }),
        platforms.end()
    );

    if (spawnTimer >= 5.0f && canSpawnPlatform()) {
        spawnPlatformPattern();
    }

    difficultyTimer += deltaTime * 1000;
    if (difficultyTimer >= difficultyIncreaseInterval) {
        increaseDifficulty(0.5f);
        difficultyTimer = 0;
    }
}

bool PlatformManager::canSpawnPlatform() const {
    if (platforms.empty()) {
        return true;
    }

    int furthestX = 0;
    for (const auto& platform : platforms) {
        int platformRight = platform.rect.x + platform.rect.w;
        if (platformRight > furthestX) {
            furthestX = platformRight;
        }
    }

    int distanceToRightEdge = SCREEN_WIDTH - furthestX;
    float t = (difficulty - 1.0f) / 9.0f;
    float minDistance = 700.0f - t * 400.0f;
    if (minDistance < 300.0f) minDistance = 300.0f;

    return distanceToRightEdge >= minDistance;
}

void PlatformManager::increaseDifficulty(float amount) {
    difficulty += amount;
    if (difficulty > 10.0f) {
        difficulty = 10.0f;
    }

    float t = (difficulty - 1.0f) / 9.0f;
    scrollSpeed = 4.0f + t * 6.0f;
    float minDistance = 700.0f - t * 400.0f;
    if (minDistance < 300.0f) minDistance = 300.0f;

    std::cout << "Difficulty increased to: " << difficulty
              << ", Scroll Speed: " << scrollSpeed
              << ", Min Distance: " << minDistance << std::endl;
}

int PlatformManager::findValidYForGrass(int x, int kongHeight) const {
    std::vector<int> validPositions;

    validPositions.push_back(GROUND_LEVEL - kongHeight);

    for (const auto& platform : platforms) {
        if (platform.active && (platform.type == PlatformType::LAND_MID || platform.type == PlatformType::LAND_SMALL)) {
            if (x >= platform.rect.x - 20 && x <= platform.rect.x + platform.rect.w + 20) {
                int landHeight = (platform.type == PlatformType::LAND_MID) ? 155 : 159;
                validPositions.push_back(platform.rect.y - landHeight - kongHeight);
            }
        }
    }

    for (const auto& platform : platforms) {
        if (platform.active && (platform.type == PlatformType::GRASS_BIG ||
                               platform.type == PlatformType::GRASS_MID ||
                               platform.type == PlatformType::GRASS_SUPERBIG)) {
            if (x >= platform.rect.x - 20 && x <= platform.rect.x + platform.rect.w + 20) {
                validPositions.push_back(platform.rect.y - kongHeight - 34);
            }
        }
    }

    return validPositions[rand() % validPositions.size()];
}

bool PlatformManager::isValidPositionForObstacle(int x, int y, int width, int height) const {
    const auto& obstacles = obstacleManager.getObstacles();
    for (const auto& obstacle : obstacles) {
        int safeDistance = 50;
        if (x + width >= obstacle.rect.x - safeDistance &&
            x <= obstacle.rect.x + obstacle.rect.w + safeDistance &&
            y + height >= obstacle.rect.y &&
            y <= obstacle.rect.y + obstacle.rect.h) {
            return false;
        }
    }

    for (const auto& platform : platforms) {
        if (platform.active && (platform.type == PlatformType::GRASS_BIG ||
                               platform.type == PlatformType::GRASS_MID ||
                               platform.type == PlatformType::GRASS_SUPERBIG ||
                               platform.type == PlatformType::LAND_MID ||
                               platform.type == PlatformType::LAND_SMALL)) {
            if (x >= platform.rect.x && x + width <= platform.rect.x + platform.rect.w &&
                y + height == platform.rect.y) {
                return true;
            }
        }
    }

    if (y + height == GROUND_LEVEL) {
        return true;
    }

    return false;
}

void PlatformManager::render(Graphics* graphics) {
    for (const auto& platform : platforms) {
        if (platform.active) {
            graphics->renderTexture(platform.texture, platform.rect.x, platform.rect.y);
        }
    }
}

const std::vector<Platform>& PlatformManager::getPlatforms() const {
    return platforms;
}

void PlatformManager::setScrollSpeed(float speed) {
    scrollSpeed = speed;
}

void PlatformManager::clear() {
    platforms.clear();
}

float PlatformManager::getDifficulty() const {
    return difficulty;
}
