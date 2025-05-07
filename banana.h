#ifndef BANANA_H
#define BANANA_H

#include "common_func.h"
#include "platform.h"
#include "graphics.h"

class PlatformManager;
class ObstacleManager;

enum class BananaType {
    NORMAL
};

class Banana {
public:
    Banana(SDL_Texture* tex, BananaType t, int x, int y);
    SDL_Texture* texture;
    BananaType type;
    bool active;
    SDL_Rect rect;
    int value;

private:
};

class BananaManager {
public:
    BananaManager(std::map<BananaType, SDL_Texture*> textures, const PlatformManager& platformMgr, const ObstacleManager& obstacleMgr, Graphics& graphics);
    ~BananaManager();
    void update(float deltaTime);
    void render();
    bool checkCollision(int playerX, int playerY, int playerRadius, int& score, bool& magnetActivated);
    void spawnBananas();
    void spawnOnPlatform(const Platform& platform);
    void spawnOnGround(int startX, int endX);
    bool isValidBananaPosition(int x, int y, int width, int height) const;
    bool isNearCollectedPosition(int x, int y) const;
    void setScrollSpeed(float speed);
    void setDifficultyFactor(float difficulty);
    void clear();
    bool checkObstacleCollision(const SDL_Rect& bananaRect, const ObstacleManager& obstacleMgr) const;

private:
    std::map<BananaType, SDL_Texture*> bananaTextures;
    std::vector<Banana> bananas;
    std::set<int> spawnedPlatforms;
    std::vector<SDL_Point> collectedPositions;
    const PlatformManager& platformManager;
    const ObstacleManager& obstacleManager;
    Graphics& graphics;
    float scrollSpeed;
    float spawnTimer;
    float spawnInterval;
    float difficultyFactor;
    float lastSpawnTime;
};

#endif // BANANA_H
