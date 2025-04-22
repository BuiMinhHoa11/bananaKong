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
            rect.w = 498;
            rect.h = 38;
            break;
        case PlatformType::GRASS_MID:
            rect.w = 614;
            rect.h = 34;
            break;
        case PlatformType::GRASS_SUPERBIG:
            rect.w = 728;
            rect.h = 34;
            break;
        case PlatformType::LAND_MID:
            rect.w = 867;
            rect.h = 155;
            break;
        case PlatformType::LAND_SMALL:
            rect.w = 750;
            rect.h = 159;
            break;
        case PlatformType::VINE:
            rect.w = 697;
            rect.h = 487;
            break;
    }

    rect.x = x;
    rect.y = y;
}

PlatformManager::PlatformManager(std::map<PlatformType, SDL_Texture*> textures, ObstacleManager& obsManager)
    : obstacleManager(obsManager) {
    platformTextures = textures;
    scrollSpeed = 4.0f;
    spawnDelay = 8000; //cứ mỗi 8 giây, một nền tảng mới sẽ được tạo
    spawnTimer = 0;
    difficulty = 1.0f;
    difficultyTimer = 0;
    difficultyIncreaseInterval = 25000;
    minPlatformDistance = SCREEN_WIDTH / 3.75;
    initialPlatformDistance = SCREEN_WIDTH * 4.5;
    srand(static_cast<unsigned>(time(nullptr)));
}

void PlatformManager::update(float deltaTime) {
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

    spawnTimer += static_cast<int>(deltaTime * 1000);
    if (spawnTimer >= spawnDelay && canSpawnPlatform()) {
        spawnPlatformPattern();
        spawnTimer = 0;
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

    float t = (difficulty - 1.0f) / 10.0f;
    float requiredDistance = initialPlatformDistance - (initialPlatformDistance - minPlatformDistance) * t;
    if (requiredDistance < minPlatformDistance) {
        requiredDistance = minPlatformDistance;
    }

    return (SCREEN_WIDTH - furthestX) >= requiredDistance;
}

void PlatformManager::increaseDifficulty(float amount) {
    difficulty += amount;
    if (difficulty > 10.0f) {
        difficulty = 10.0f;
    }

    float t = (difficulty - 1.0f) / 10.0f;
    scrollSpeed = 4.0f + t * 6.0f;
    spawnDelay = static_cast<int>(8000 - t * 5000);
    if (spawnDelay < 5000) {
        spawnDelay = 5000;
    }

    std::cout << "Difficulty increased to: " << difficulty
              << ", Speed: " << scrollSpeed
              << ", Spawn Delay: " << spawnDelay << std::endl;
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
            renderDebugCollision(graphics, platform);
        }
    }
}

void PlatformManager::renderDebugCollision(Graphics* graphics, const Platform& platform) {
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(graphics->getRenderer(), &r, &g, &b, &a);
    SDL_SetRenderDrawColor(graphics->getRenderer(), 0, 255, 0, 128);
    SDL_Rect outlineRect = platform.rect;
    SDL_RenderDrawRect(graphics->getRenderer(), &outlineRect);
    SDL_SetRenderDrawColor(graphics->getRenderer(), r, g, b, a);
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
