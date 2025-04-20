/*#ifndef PLATFORM_H_
#define PLATFORM_H_

#include "obstacle.h"
#include "common_func.h"
#include "graphics.h"
#include <vector>
#include <map>

#ifndef KONG_HEIGHT
#endif

// Định nghĩa GROUND_LEVEL
const int GROUND_LEVEL = 730;
const int KONG_HEIGHT = 149;

enum class PlatformType {
    GRASS_BIG,
    GRASS_MID,
    GRASS_SUPERBIG,
    LAND_MID,
    LAND_SMALL,
    VINE
};

enum class PlatformPattern {
    SINGLE,      // Platform đơn lẻ
    STAIR_UP,    // Các platform xếp như cầu thang đi lên
    PARALLEL,    // Nhiều platform song song
    GAP,
    BARREL       // Các platform có khoảng cách (để nhảy)
};

struct Platform {
    SDL_Rect rect;
    SDL_Texture* texture;
    PlatformType type;
    bool active; // Để biết nền tảng có hiển thị trên màn hình không

    Platform(SDL_Texture* tex, PlatformType t, int x, int y);
};

class PlatformManager {
private:
    std::map<PlatformType, SDL_Texture*> platformTextures;
    std::vector<Platform> platforms;
    float difficulty;
    float scrollSpeed; // Tốc độ di chuyển của platform
    int spawnDelay;  // Thời gian giữa mỗi lần sinh platform
    int spawnTimer;  // Đếm thời gian để sinh platform tiếp theo
    ObstacleManager& obstacleManager;
    // Thêm các phương thức mới
    bool hasLandPlatformAt(int x, int y, int tolerance = 5) const;
    bool hasGrassPlatformAt(int x, int y, int tolerance = 5) const;
    int findValidYForGrass(int x, int kongHeight) const;

public:
    PlatformManager(std::map<PlatformType, SDL_Texture*> textures, ObstacleManager& obsManager);
    void update(float deltaTime);
    void render(Graphics* graphics);
    void renderDebugCollision(Graphics* graphics, const Platform& platform);
    const std::vector<Platform>& getPlatforms() const;
    void setScrollSpeed(float speed);
    void clear();
    bool isValidPositionForObstacle(int x, int y, int width, int height) const;

    void spawnPlatformPattern();
    void spawnSinglePlatform();
    void spawnStairUpPattern();
    void spawnParallelPattern();
    void spawnGapPattern();
    void spawnBarrelPattern(ObstacleManager& obstacleManager);

    void increaseDifficulty(float amount);
    float getDifficulty() const;
    float difficultyTimer;
    float difficultyIncreaseInterval;
};

#endif // PLATFORM_H_
*/

#ifndef PLATFORM_H_
#define PLATFORM_H_

#include "obstacle.h"
#include "common_func.h"
#include "graphics.h"
#include <vector>
#include <map>

// Định nghĩa GROUND_LEVEL và KONG_HEIGHT
const int GROUND_LEVEL = 730;
const int KONG_HEIGHT = 149;

enum class PlatformType {
    GRASS_BIG,
    GRASS_MID,
    GRASS_SUPERBIG,
    LAND_MID,
    LAND_SMALL,
    VINE
};

enum class PlatformPattern {
    SINGLE,      // Platform đơn lẻ
    STAIR_UP,    // Các platform xếp như cầu thang đi lên
    PARALLEL,    // Nhiều platform song song
    GAP,         // Các platform có khoảng cách (để nhảy)
    BARREL,      // Platform có chướng ngại vật
    LAND_OBSTACLE // Platform đất với chướng ngại vật
};

struct Platform {
    SDL_Rect rect;
    SDL_Texture* texture;
    PlatformType type;
    bool active; // Để biết nền tảng có hiển thị trên màn hình không

    Platform(SDL_Texture* tex, PlatformType t, int x, int y);
};

class PlatformManager {
private:
    std::map<PlatformType, SDL_Texture*> platformTextures;
    std::vector<Platform> platforms;
    float difficulty;
    float scrollSpeed; // Tốc độ di chuyển của platform
    int spawnDelay;  // Thời gian giữa mỗi lần sinh platform
    int spawnTimer;  // Đếm thời gian để sinh platform tiếp theo
    ObstacleManager& obstacleManager;
    // Giữ lại các phương thức cần thiết
    int findValidYForGrass(int x, int kongHeight) const;

public:
    PlatformManager(std::map<PlatformType, SDL_Texture*> textures, ObstacleManager& obsManager);
    void update(float deltaTime);
    void render(Graphics* graphics);
    void renderDebugCollision(Graphics* graphics, const Platform& platform);
    const std::vector<Platform>& getPlatforms() const;
    void setScrollSpeed(float speed);
    void clear();
    bool isValidPositionForObstacle(int x, int y, int width, int height) const;

    void spawnPlatformPattern();
    void spawnSinglePlatform();
    void spawnStairUpPattern();
    void spawnParallelPattern();
    void spawnGapPattern();
    void spawnBarrelPattern(ObstacleManager& obstacleManager);
    void spawnLandObstaclePattern(ObstacleManager& obstacleManager); // Hàm mới

    void increaseDifficulty(float amount);
    float getDifficulty() const;
    float difficultyTimer;
    float difficultyIncreaseInterval;
};

#endif // PLATFORM_H_

