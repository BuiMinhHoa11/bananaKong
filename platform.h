#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL.h>
#include <map>
#include <vector>
#include "obstacle.h"
#include "common_func.h"

enum class PlatformType {
    GRASS_BIG,
    GRASS_MID,
    GRASS_SUPERBIG,
    LAND_MID,
    LAND_SMALL,
};

class Graphics;

class Platform {
public:
    Platform(SDL_Texture* tex, PlatformType t, int x, int y);
    SDL_Texture* texture;
    PlatformType type;
    SDL_Rect rect;
    bool active;
};

class PlatformManager {
public:
    PlatformManager(std::map<PlatformType, SDL_Texture*> textures, ObstacleManager& obsManager);
    void update(float deltaTime);
    void render(Graphics* graphics);
    void clear();
    void setScrollSpeed(float speed);
    float getDifficulty() const;
    const std::vector<Platform>& getPlatforms() const;
    ObstacleManager& getObstacleManager() { return obstacleManager; }
    const std::map<PlatformType, SDL_Texture*>& getPlatformTextures() const { return platformTextures; }
    float getScrollSpeed() const { return scrollSpeed; }

    void spawnPlatformPattern();
    PlatformType getRandomGrassType() const;
    void spawnLadderType1();
    void spawnLadderType2();
    void spawnLadderType3();
    void spawnGapType1();
    void spawnGapType2();
    void spawnGapType3();

private:
    std::map<PlatformType, SDL_Texture*> platformTextures;
    std::vector<Platform> platforms;
    ObstacleManager& obstacleManager;
    float scrollSpeed;
    float spawnTimer;
    float difficulty;
    float difficultyTimer;
    int difficultyIncreaseInterval;

    bool canSpawnPlatform() const;
    void increaseDifficulty(float amount);
    int findValidYForGrass(int x, int kongHeight) const;
    bool isValidPositionForObstacle(int x, int y, int width, int height) const;
    //void renderDebugCollision(Graphics* graphics, const Platform& platform);
};

#endif
