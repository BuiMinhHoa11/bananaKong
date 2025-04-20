#include "platform.h"

void PlatformManager::spawnPlatformPattern() {
    int patternRoll = rand() % 100;
    PlatformPattern pattern;

    if (difficulty < 2.0f) { // Mức dễ
        if (patternRoll < 25) pattern = PlatformPattern::SINGLE;        // 25%
        else if (patternRoll < 40) pattern = PlatformPattern::STAIR_UP; // 15%
        else if (patternRoll < 60) pattern = PlatformPattern::PARALLEL; // 20%
        else if (patternRoll < 70) pattern = PlatformPattern::LAND_OBSTACLE; // 30%
        else pattern = PlatformPattern::BARREL;                        // 30%
    } else if (difficulty < 5.0f) { // Mức trung bình
        if (patternRoll < 20) pattern = PlatformPattern::SINGLE;        // 20%
        else if (patternRoll < 30) pattern = PlatformPattern::STAIR_UP; // 10%
        else if (patternRoll < 50) pattern = PlatformPattern::PARALLEL; // 20%
        else if (patternRoll < 70) pattern = PlatformPattern::LAND_OBSTACLE; // 40%
        else if (patternRoll < 90) pattern = PlatformPattern::GAP;      // 20%
        else pattern = PlatformPattern::BARREL;                        // 10%
    } else { // Mức khó
        if (patternRoll < 20) pattern = PlatformPattern::SINGLE;        // 20%
        else if (patternRoll < 30) pattern = PlatformPattern::STAIR_UP; // 10%
        else if (patternRoll < 50) pattern = PlatformPattern::PARALLEL; // 20%
        else if (patternRoll < 70) pattern = PlatformPattern::LAND_OBSTACLE; // 20%
        else if (patternRoll < 90) pattern = PlatformPattern::GAP;      // 20%
        else pattern = PlatformPattern::BARREL;                        // 10%
    }

    switch (pattern) {
        case PlatformPattern::SINGLE: spawnSinglePlatform(); break;
        case PlatformPattern::STAIR_UP: spawnStairUpPattern(); break;
        case PlatformPattern::PARALLEL: spawnParallelPattern(); break;
        case PlatformPattern::GAP: spawnGapPattern(); break;
        case PlatformPattern::BARREL: spawnBarrelPattern(obstacleManager); break;
        case PlatformPattern::LAND_OBSTACLE: spawnLandObstaclePattern(obstacleManager); break;
    }
}

void PlatformManager::spawnSinglePlatform() {
    std::vector<PlatformType> grassTypes = {
        PlatformType::GRASS_BIG, PlatformType::GRASS_MID, PlatformType::GRASS_SUPERBIG
    };
    std::vector<PlatformType> landTypes = {
        PlatformType::LAND_MID, PlatformType::LAND_SMALL
    };

    bool spawnGrass = (rand() % 100) < 20;
    PlatformType randomType = spawnGrass ? grassTypes[rand() % grassTypes.size()] : landTypes[rand() % landTypes.size()];

    if (platformTextures.find(randomType) != platformTextures.end()) {
        int xPos = SCREEN_WIDTH;
        int yPos;

        if (randomType == PlatformType::LAND_MID || randomType == PlatformType::LAND_SMALL) {
            yPos = GROUND_LEVEL;
        } else {
            yPos = findValidYForGrass(xPos, KONG_HEIGHT);
        }

        Platform newPlatform(platformTextures[randomType], randomType, xPos, yPos);
        bool canSpawn = true;
        if (!platforms.empty()) {
            Platform& lastPlatform = platforms.back();
            if (SCREEN_WIDTH - (lastPlatform.rect.x + lastPlatform.rect.w) < 300) {
                canSpawn = false;
            }
        }

        if (canSpawn) {
            platforms.push_back(newPlatform);
        }
    }
}

void PlatformManager::spawnLandObstaclePattern(ObstacleManager& obstacleManager) {
    std::vector<PlatformType> landTypes = {
        PlatformType::LAND_MID, PlatformType::LAND_SMALL
    };
    PlatformType landType = landTypes[rand() % landTypes.size()];

    if (platformTextures.find(landType) == platformTextures.end()) {
        std::cerr << "Error: Texture for " << static_cast<int>(landType) << " not found!" << std::endl;
        return;
    }

    int xPos = SCREEN_WIDTH;
    int yPos = GROUND_LEVEL;

    bool canSpawn = true;
    if (!platforms.empty()) {
        Platform& lastPlatform = platforms.back();
        if (SCREEN_WIDTH - (lastPlatform.rect.x + lastPlatform.rect.w) < 300) {
            canSpawn = false;
        }
    }

    if (!canSpawn) {
        return;
    }

    Platform landPlatform(platformTextures[landType], landType, xPos, yPos);
    platforms.push_back(landPlatform);

    ObstacleType obstacleType = obstacleManager.getRandomObstacleType();
    SDL_Texture* obstacleTexture = obstacleManager.getTextureForType(obstacleType);

    if (obstacleTexture) {
        Obstacle newObstacle;
        newObstacle.type = obstacleType;
        newObstacle.rect.w = (obstacleType == ObstacleType::PLANE) ? 270 :
                             (obstacleType == ObstacleType::ROCK) ? 209 : 106;
        newObstacle.rect.h = 133;
        newObstacle.rect.x = xPos + 50 + (rand() % (landPlatform.rect.w - newObstacle.rect.w - 100));
        newObstacle.rect.y = 755 - newObstacle.rect.h;

        if (isValidPositionForObstacle(newObstacle.rect.x, newObstacle.rect.y, newObstacle.rect.w, newObstacle.rect.h)) {
            newObstacle.texture = obstacleTexture;
            newObstacle.speed = obstacleManager.getRandomSpeed();
            newObstacle.isPlatform = false;
            obstacleManager.addObstacle(newObstacle);
        }
    }
}

void PlatformManager::spawnBarrelPattern(ObstacleManager& obstacleManager) {
    bool useLand = (rand() % 100) < 60;
    int numLayers = (rand() % 2) + 1;

    PlatformType baseType;
    if (useLand) {
        baseType = (rand() % 2 == 0) ? PlatformType::LAND_MID : PlatformType::LAND_SMALL;
    } else {
        baseType = (numLayers == 1) ? PlatformType::GRASS_MID
                                   : ((rand() % 2 == 0) ? PlatformType::GRASS_SUPERBIG : PlatformType::GRASS_BIG);
    }

    if (platformTextures.find(baseType) == platformTextures.end()) {
        std::cerr << "Error: Texture for " << static_cast<int>(baseType) << " not found!" << std::endl;
        return;
    }

    int xPos = SCREEN_WIDTH;
    int yPos = useLand ? GROUND_LEVEL : findValidYForGrass(xPos, KONG_HEIGHT);

    bool canSpawn = true;
    if (!platforms.empty()) {
        Platform& lastPlatform = platforms.back();
        if (SCREEN_WIDTH - (lastPlatform.rect.x + lastPlatform.rect.w) < 300) {
            canSpawn = false;
        }
    }

    if (!canSpawn) {
        return;
    }

    Platform basePlatform(platformTextures[baseType], baseType, xPos, yPos);
    platforms.push_back(basePlatform);

    if (rand() % 100 < 30) {
        ObstacleType obstacleType = obstacleManager.getRandomObstacleType();
        SDL_Texture* obstacleTexture = obstacleManager.getTextureForType(obstacleType);

        if (obstacleTexture) {
            Obstacle newObstacle;
            newObstacle.type = obstacleType;
            newObstacle.rect.w = (obstacleType == ObstacleType::PLANE) ? 270 :
                                 (obstacleType == ObstacleType::ROCK) ? 209 : 106;
            newObstacle.rect.h = 133;
            newObstacle.rect.x = xPos + 50 + (rand() % (basePlatform.rect.w - newObstacle.rect.w - 100));
            newObstacle.rect.y = yPos - newObstacle.rect.h;

            if (isValidPositionForObstacle(newObstacle.rect.x, newObstacle.rect.y, newObstacle.rect.w, newObstacle.rect.h)) {
                newObstacle.texture = obstacleTexture;
                newObstacle.speed = obstacleManager.getRandomSpeed();
                newObstacle.isPlatform = false;
                obstacleManager.addObstacle(newObstacle);
            }
        }
    }

    if (numLayers == 2 && !useLand) {
        PlatformType upperType = PlatformType::GRASS_MID;
        int upperYPos = yPos - KONG_HEIGHT - 34;

        if (platformTextures.find(upperType) != platformTextures.end()) {
            Platform upperPlatform(platformTextures[upperType], upperType, xPos + 100, upperYPos);
            platforms.push_back(upperPlatform);

            if (rand() % 100 < 50) {
                ObstacleType obstacleType = obstacleManager.getRandomObstacleType();
                SDL_Texture* obstacleTexture = obstacleManager.getTextureForType(obstacleType);

                if (obstacleTexture) {
                    Obstacle newObstacle;
                    newObstacle.type = obstacleType;
                    newObstacle.rect.w = (obstacleType == ObstacleType::PLANE) ? 270 :
                                         (obstacleType == ObstacleType::ROCK) ? 209 : 106;
                    newObstacle.rect.h = 133;
                    newObstacle.rect.x = xPos + 150 + (rand() % (upperPlatform.rect.w - newObstacle.rect.w - 100));
                    newObstacle.rect.y = upperYPos - newObstacle.rect.h;

                    if (isValidPositionForObstacle(newObstacle.rect.x, newObstacle.rect.y, newObstacle.rect.w, newObstacle.rect.h)) {
                        newObstacle.texture = obstacleTexture;
                        newObstacle.speed = obstacleManager.getRandomSpeed();
                        newObstacle.isPlatform = false;
                        obstacleManager.addObstacle(newObstacle);
                    }
                }
            }
        }
    }
}

void PlatformManager::spawnStairUpPattern() {
    const int NUM_STEPS = 3 + (rand() % 2);

    std::vector<PlatformType> grassTypes = {
        PlatformType::GRASS_BIG, PlatformType::GRASS_MID, PlatformType::GRASS_SUPERBIG
    };
    PlatformType grassType = grassTypes[rand() % grassTypes.size()];

    int xPos = SCREEN_WIDTH;
    int yPos = GROUND_LEVEL;
    int stepWidth = 230;
    int stepHeight = KONG_HEIGHT;

    for (int i = 0; i < NUM_STEPS; i++) {
        if (platformTextures.find(grassType) != platformTextures.end()) {
            Platform newPlatform(platformTextures[grassType], grassType, xPos, yPos);
            platforms.push_back(newPlatform);
        }
        xPos += stepWidth;
        yPos -= stepHeight;
    }
}

void PlatformManager::spawnParallelPattern() {
    std::vector<PlatformType> grassTypes = {
        PlatformType::GRASS_BIG, PlatformType::GRASS_MID, PlatformType::GRASS_SUPERBIG
    };
    PlatformType grassType = grassTypes[rand() % grassTypes.size()];

    int xPos = SCREEN_WIDTH;
    int baseY = GROUND_LEVEL;
    int platformDistance = 280;

    if (platformTextures.find(grassType) != platformTextures.end()) {
        Platform firstPlatform(platformTextures[grassType], grassType, xPos, baseY);
        platforms.push_back(firstPlatform);
        Platform secondPlatform(platformTextures[grassType], grassType, xPos + platformDistance, baseY - KONG_HEIGHT);
        platforms.push_back(secondPlatform);
        Platform thirdPlatform(platformTextures[grassType], grassType, xPos + platformDistance * 2, baseY - KONG_HEIGHT * 2);
        platforms.push_back(thirdPlatform);
    }
}

void PlatformManager::spawnGapPattern() {
    const int NUM_PLATFORMS = 3;

    std::vector<PlatformType> grassTypes = {
        PlatformType::GRASS_BIG, PlatformType::GRASS_MID, PlatformType::GRASS_SUPERBIG
    };
    PlatformType grassType = grassTypes[rand() % grassTypes.size()];

    int xPos = SCREEN_WIDTH;
    int yPos = GROUND_LEVEL;
    int gapSize = 200 + (rand() % 100);

    if (platformTextures.find(grassType) != platformTextures.end()) {
        for (int i = 0; i < NUM_PLATFORMS; i++) {
            Platform newPlatform(platformTextures[grassType], grassType, xPos, yPos);
            platforms.push_back(newPlatform);
            xPos += 350 + gapSize;
        }
    }
}
