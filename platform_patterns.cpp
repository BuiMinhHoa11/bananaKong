//platform_patterns.cpp
#include "platform.h"
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <random>

void PlatformManager::spawnPlatformPattern() {
    int roll = rand() % 100;
    int patternType;

    if (difficulty <= 2.0f) {
        if (roll < 40) patternType = 1;
        else if (roll < 80) patternType = 4;
        else patternType = 2;
    } else if (difficulty <= 4.5f) {
        if (roll < 20) patternType = 5;
        else if (roll < 40) patternType = 4;
        else if (roll < 60) patternType = 2;
        else if (roll < 80) patternType = 3;
        else patternType = 6;
    } else if (difficulty <= 7.0f) {
        if (roll < 30) patternType = 2;
        else if (roll < 60) patternType = 5;
        else if (roll < 80) patternType = 4;
        else patternType = 6;
    } else {
        if (roll < 45) patternType = 6;
        else if (roll < 75) patternType = 3;
        else if (roll < 95) patternType = 5;
        else patternType = 2;
    }

    switch (patternType) {
        case 1: spawnLadderType1(); break;
        case 2: spawnLadderType2(); break;
        case 3: spawnLadderType3(); break;
        case 4: spawnGapType1(); break;
        case 5: spawnGapType2(); break;
        case 6: spawnGapType3(); break;
    }
}

PlatformType PlatformManager::getRandomGrassType() const {
    std::vector<PlatformType> grassTypes = {
        PlatformType::GRASS_BIG, PlatformType::GRASS_MID, PlatformType::GRASS_SUPERBIG
    };
    return grassTypes[rand() % grassTypes.size()];
}

void PlatformManager::spawnLadderType1() {
    PlatformType grassType = getRandomGrassType();
    int xPos = SCREEN_WIDTH;
    int yPos = GROUND_LEVEL - KONG_HEIGHT;

    if (platformTextures.find(grassType) != platformTextures.end()) {
        Platform grassPlatform(platformTextures.at(grassType), grassType, xPos, yPos);
        platforms.push_back(grassPlatform);

        if (rand() % 100 < 70) {
            bool spawnAbove = (rand() % 100 < 50);
            ObstacleType obstacleType = obstacleManager.getRandomObstacleType();
            SDL_Texture* obstacleTexture = obstacleManager.getTextureForType(obstacleType);

            if (obstacleTexture) {
                int obstacleWidth = (obstacleType == ObstacleType::PLANE) ? 270 :
                                    (obstacleType == ObstacleType::ROCK) ? 209 : 106;
                int obstacleHeight = 133;
                int obstacleX = xPos + 200 + (rand() % (grassPlatform.rect.w - obstacleWidth - 100));
                int obstacleY = spawnAbove ? yPos - obstacleHeight : GROUND_LEVEL - obstacleHeight;

                if (isValidPositionForObstacle(obstacleX, obstacleY, obstacleWidth, obstacleHeight)) {
                    Obstacle newObstacle;
                    newObstacle.type = obstacleType;
                    newObstacle.rect = {obstacleX, obstacleY, obstacleWidth, obstacleHeight};
                    newObstacle.texture = obstacleTexture;
                    newObstacle.speed = scrollSpeed / 5.0f;
                    newObstacle.isPlatform = true; // Cho phép đi lên
                    obstacleManager.addObstacle(newObstacle);
                }
            }
        }
    }
}

void PlatformManager::spawnLadderType2() {
    PlatformType grassType1 = getRandomGrassType();
    PlatformType grassType2 = getRandomGrassType();
    int xPos = SCREEN_WIDTH;
    int tier1Y = GROUND_LEVEL - KONG_HEIGHT;
    int tier2Y = tier1Y - KONG_HEIGHT - 34;

    if (platformTextures.find(grassType1) != platformTextures.end()) {
        Platform tier1Platform(platformTextures.at(grassType1), grassType1, xPos, tier1Y);
        platforms.push_back(tier1Platform);

        if (platformTextures.find(grassType2) != platformTextures.end()) {
            Platform tier2Platform(platformTextures.at(grassType2), grassType2, xPos + 100, tier2Y);
            platforms.push_back(tier2Platform);

            if (rand() % 100 < 80) {
                int numObstacles = (rand() % 100 < 30) ? 2 : 1;
                for (int i = 0; i < numObstacles; i++) {
                    ObstacleType obstacleType = obstacleManager.getRandomObstacleType();
                    SDL_Texture* obstacleTexture = obstacleManager.getTextureForType(obstacleType);

                    if (obstacleTexture) {
                        int obstacleWidth = (obstacleType == ObstacleType::PLANE) ? 270 :
                                            (obstacleType == ObstacleType::ROCK) ? 209 : 106;
                        int obstacleHeight = 133;
                        int obstacleX, obstacleY;

                        int position = rand() % 3;
                        if (position == 0 || (i == 1 && numObstacles == 2)) {
                            obstacleX = xPos + 400 + (rand() % (tier1Platform.rect.w - obstacleWidth - 100));
                            obstacleY = GROUND_LEVEL - obstacleHeight;
                        } else if (position == 1) {
                            obstacleX = xPos + 400 + (rand() % (tier1Platform.rect.w - obstacleWidth - 100));
                            obstacleY = tier1Y - obstacleHeight;
                        } else {
                            obstacleX = xPos + 450 + (rand() % (tier2Platform.rect.w - obstacleWidth - 100));
                            obstacleY = tier2Y - obstacleHeight;
                        }

                        if (isValidPositionForObstacle(obstacleX, obstacleY, obstacleWidth, obstacleHeight)) {
                            Obstacle newObstacle;
                            newObstacle.type = obstacleType;
                            newObstacle.rect = {obstacleX, obstacleY, obstacleWidth, obstacleHeight};
                            newObstacle.texture = obstacleTexture;
                            newObstacle.speed = scrollSpeed / 5.0f;
                            newObstacle.isPlatform = true; // Cho phép đi lên
                            obstacleManager.addObstacle(newObstacle);
                        }
                    }
                }
            }
        }
    }
}

void PlatformManager::spawnLadderType3() {
    PlatformType grassType = getRandomGrassType();
    int xPos = SCREEN_WIDTH;
    int tier1Y = 739;
    int tier2Y = 590;
    int tier3Y = 441;

    if (platformTextures.find(grassType) != platformTextures.end()) {
        Platform tier1Platform(platformTextures.at(grassType), grassType, xPos, tier1Y);
        platforms.push_back(tier1Platform);

        Platform tier2Platform(platformTextures.at(grassType), grassType, xPos + 200, tier2Y);
        platforms.push_back(tier2Platform);

        Platform tier3Platform(platformTextures.at(grassType), grassType, xPos + 300, tier3Y);
        platforms.push_back(tier3Platform);

        std::vector<int> tiers = {0, 1, 2};
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(tiers.begin(), tiers.end(), g);

        for (int i = 0; i < 2; i++) {
            ObstacleType obstacleType = obstacleManager.getRandomObstacleType();
            SDL_Texture* obstacleTexture = obstacleManager.getTextureForType(obstacleType);

            if (obstacleTexture) {
                int obstacleWidth = (obstacleType == ObstacleType::PLANE) ? 270 :
                                    (obstacleType == ObstacleType::ROCK) ? 209 : 106;
                int obstacleHeight = 133;
                int obstacleX, obstacleY;

                int tier = tiers[i];
                if (tier == 0) {
                    obstacleX = xPos + 200 + (rand() % (tier1Platform.rect.w - obstacleWidth - 100));
                    obstacleY = tier1Y - obstacleHeight;
                } else if (tier == 1) {
                    obstacleX = xPos + 350 + (rand() % (tier2Platform.rect.w - obstacleWidth - 100));
                    obstacleY = tier2Y - obstacleHeight;
                } else {
                    obstacleX = xPos + 650 + (rand() % (tier3Platform.rect.w - obstacleWidth - 100));
                    obstacleY = tier3Y - obstacleHeight;
                }

                if (isValidPositionForObstacle(obstacleX, obstacleY, obstacleWidth, obstacleHeight)) {
                    Obstacle newObstacle;
                    newObstacle.type = obstacleType;
                    newObstacle.rect = {obstacleX, obstacleY, obstacleWidth, obstacleHeight};
                    newObstacle.texture = obstacleTexture;
                    newObstacle.speed = scrollSpeed / 5.0f;
                    newObstacle.isPlatform = true; // Cho phép đi lên
                    obstacleManager.addObstacle(newObstacle);
                }
            }
        }
    }
}

void PlatformManager::spawnGapType1() {
    PlatformType landType = (rand() % 2 == 0) ? PlatformType::LAND_MID : PlatformType::LAND_SMALL;
    int xPos = SCREEN_WIDTH;
    int landHeight = (landType == PlatformType::LAND_MID) ? 155 : 159;
    int yPos = GROUND_LEVEL - landHeight;

    if (platformTextures.find(landType) != platformTextures.end()) {
        Platform landPlatform(platformTextures.at(landType), landType, xPos, yPos);
        platforms.push_back(landPlatform);

        if (rand() % 100 < 70) {
            bool spawnAbove = (rand() % 100 < 50);
            ObstacleType obstacleType = obstacleManager.getRandomObstacleType();
            SDL_Texture* obstacleTexture = obstacleManager.getTextureForType(obstacleType);

            if (obstacleTexture) {
                int obstacleWidth = (obstacleType == ObstacleType::PLANE) ? 270 :
                                    (obstacleType == ObstacleType::ROCK) ? 209 : 106;
                int obstacleHeight = 133;
                int obstacleX = xPos + 350 + (rand() % (landPlatform.rect.w - obstacleWidth - 100));
                int obstacleY = spawnAbove ? yPos - obstacleHeight : GROUND_LEVEL - obstacleHeight;

                if (isValidPositionForObstacle(obstacleX, obstacleY, obstacleWidth, obstacleHeight)) {
                    Obstacle newObstacle;
                    newObstacle.type = obstacleType;
                    newObstacle.rect = {obstacleX, obstacleY, obstacleWidth, obstacleHeight};
                    newObstacle.texture = obstacleTexture;
                    newObstacle.speed = scrollSpeed / 5.0f;
                    newObstacle.isPlatform = true; // Cho phép đi lên
                    obstacleManager.addObstacle(newObstacle);
                }
            }
        }
    }
}

void PlatformManager::spawnGapType2() {
    PlatformType landType = (rand() % 2 == 0) ? PlatformType::LAND_MID : PlatformType::LAND_SMALL;
    PlatformType grassType = getRandomGrassType();
    int xPos = SCREEN_WIDTH;
    int landHeight = (landType == PlatformType::LAND_MID) ? 155 : 159;
    int landY = GROUND_LEVEL - landHeight;
    int grassY = landY - KONG_HEIGHT;

    if (platformTextures.find(landType) != platformTextures.end()) {
        Platform landPlatform(platformTextures.at(landType), landType, xPos, landY);
        platforms.push_back(landPlatform);

        if (platformTextures.find(grassType) != platformTextures.end()) {
            Platform grassPlatform(platformTextures.at(grassType), grassType, xPos + 100, grassY);
            platforms.push_back(grassPlatform);

            if (rand() % 100 < 80) {
                int numObstacles = (rand() % 100 < 30) ? 2 : 1;
                for (int i = 0; i < numObstacles; i++) {
                    ObstacleType obstacleType = obstacleManager.getRandomObstacleType();
                    SDL_Texture* obstacleTexture = obstacleManager.getTextureForType(obstacleType);

                    if (obstacleTexture) {
                        int obstacleWidth = (obstacleType == ObstacleType::PLANE) ? 270 :
                                            (obstacleType == ObstacleType::ROCK) ? 209 : 106;
                        int obstacleHeight = 133;
                        int obstacleX, obstacleY;

                        int position = rand() % 3;
                        if (position == 0 || (i == 1 && numObstacles == 2)) {
                            obstacleX = xPos + 100 + (rand() % (landPlatform.rect.w - obstacleWidth - 100));
                            obstacleY = GROUND_LEVEL - obstacleHeight;
                        } else if (position == 1) {
                            obstacleX = xPos + 200 + (rand() % (landPlatform.rect.w - obstacleWidth - 100));
                            obstacleY = landY - obstacleHeight;
                        } else {
                            obstacleX = xPos + 300 + (rand() % (grassPlatform.rect.w - obstacleWidth - 100));
                            obstacleY = grassY - obstacleHeight;
                        }

                        if (isValidPositionForObstacle(obstacleX, obstacleY, obstacleWidth, obstacleHeight)) {
                            Obstacle newObstacle;
                            newObstacle.type = obstacleType;
                            newObstacle.rect = {obstacleX, obstacleY, obstacleWidth, obstacleHeight};
                            newObstacle.texture = obstacleTexture;
                            newObstacle.speed = scrollSpeed / 5.0f;
                            newObstacle.isPlatform = true; // Cho phép đi lên
                            obstacleManager.addObstacle(newObstacle);
                        }
                    }
                }
            }
        }
    }
}

void PlatformManager::spawnGapType3() {
    PlatformType landType = (rand() % 2 == 0) ? PlatformType::LAND_MID : PlatformType::LAND_SMALL;
    PlatformType grassType1 = getRandomGrassType();
    PlatformType grassType2 = getRandomGrassType();
    int xPos = SCREEN_WIDTH;
    int landHeight = (landType == PlatformType::LAND_MID) ? 155 : 159;
    int landY = GROUND_LEVEL - landHeight;
    int tier1Y = landY - KONG_HEIGHT;
    int tier2Y = tier1Y - KONG_HEIGHT - 34;

    if (platformTextures.find(landType) != platformTextures.end()) {
        Platform landPlatform(platformTextures.at(landType), landType, xPos, landY);
        platforms.push_back(landPlatform);

        if (platformTextures.find(grassType1) != platformTextures.end()) {
            Platform tier1Platform(platformTextures.at(grassType1), grassType1, xPos + 100, tier1Y);
            platforms.push_back(tier1Platform);

            if (platformTextures.find(grassType2) != platformTextures.end()) {
                Platform tier2Platform(platformTextures.at(grassType2), grassType2, xPos + 200, tier2Y);
                platforms.push_back(tier2Platform);

                if (rand() % 100 < 90) {
                    int numObstacles = (rand() % 100 < 50) ? 2 : 1;
                    std::vector<int> positions = {0, 1, 2, 3};

                    std::random_device rd;
                    std::mt19937 g(rd());
                    std::shuffle(positions.begin(), positions.end(), g);

                    for (int i = 0; i < numObstacles; i++) {
                        ObstacleType obstacleType = obstacleManager.getRandomObstacleType();
                        SDL_Texture* obstacleTexture = obstacleManager.getTextureForType(obstacleType);

                        if (obstacleTexture) {
                            int obstacleWidth = (obstacleType == ObstacleType::PLANE) ? 270 :
                                                (obstacleType == ObstacleType::ROCK) ? 209 : 106;
                            int obstacleHeight = 133;
                            int obstacleX, obstacleY;

                            int pos = positions[i];
                            if (pos == 0) {
                                obstacleX = xPos + 100 + (rand() % (landPlatform.rect.w - obstacleWidth - 100));
                                obstacleY = GROUND_LEVEL - obstacleHeight;
                            } else if (pos == 1) {
                                obstacleX = xPos + 100 + (rand() % (landPlatform.rect.w - obstacleWidth - 100));
                                obstacleY = landY - obstacleHeight;
                            } else if (pos == 2) {
                                obstacleX = xPos + 350 + (rand() % (tier1Platform.rect.w - obstacleWidth - 100));
                                obstacleY = tier1Y - obstacleHeight;
                            } else {
                                obstacleX = xPos + 500 + (rand() % (tier2Platform.rect.w - obstacleWidth - 100));
                                obstacleY = tier2Y - obstacleHeight;
                            }

                            if (isValidPositionForObstacle(obstacleX, obstacleY, obstacleWidth, obstacleHeight)) {
                                Obstacle newObstacle;
                                newObstacle.type = obstacleType;
                                newObstacle.rect = {obstacleX, obstacleY, obstacleWidth, obstacleHeight};
                                newObstacle.texture = obstacleTexture;
                                newObstacle.speed = scrollSpeed / 5.0f;
                                newObstacle.isPlatform = true; // Cho phép đi lên
                                obstacleManager.addObstacle(newObstacle);
                            }
                        }
                    }
                }
            }
        }
    }
}
