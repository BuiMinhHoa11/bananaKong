/*#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <vector>
#include <map>
#include "graphics.h"

enum class ObstacleType {
    ROCK,
    SPIKE,
    HOUSE
};

struct Obstacle {
    ObstacleType type;
    SDL_Rect rect;
    SDL_Texture* texture;
    float speed;
    bool isPlatform;

    void move(float baseSpeed) {
        rect.x -= static_cast<int>(baseSpeed * speed);
    }

    bool isOffScreen() const {
        return rect.x + rect.w < 0;
    }
};

class ObstacleManager {
private:
    std::vector<Obstacle> obstacles;
    std::map<ObstacleType, SDL_Texture*> textureMap;
    float timeSinceLastSpawn;
    float baseSpawnInterval;
    float minSpawnInterval;
    float difficulty;
    float getRandomSpawnInterval() const;
    ObstacleType getRandomObstacleType() const;
    float getRandomSpeed() const;
    int getNumberToSpawn() const;
    int getRandomXOffset(int index) const;
    SDL_Rect adjustCollisionBox(const SDL_Rect& obsBox, ObstacleType type) const; // Thêm khai báo hàm này
public:
    ObstacleManager(const std::map<ObstacleType, SDL_Texture*>& textures);
    ~ObstacleManager();
    void update(float deltaTime);
    void spawn();
    const std::vector<Obstacle>& getObstacles() const { return obstacles; }
    void clear() { obstacles.clear(); }
    void setDifficulty(float difficulty) { this->difficulty = difficulty; }
    float getDifficulty() const { return difficulty; }
    bool checkCollision(const SDL_Rect& playerBox) const;
    void renderDebugCollision(Graphics* graphics) const;
};

#endif
*/

#ifndef OBSTACLE_H
#define OBSTACLE_H
#include <vector>
#include <map>
#include "graphics.h"

enum class ObstacleType {
    ROCK,
    SPIKE,
    HOUSE
};

struct Obstacle {
    ObstacleType type;
    SDL_Rect rect;
    SDL_Texture* texture;
    float speed;
    bool isPlatform;

    void move(float baseSpeed) {
        rect.x -= static_cast<int>(baseSpeed * speed);
    }

    bool isOffScreen() const {
        return rect.x + rect.w < 0;
    }
};

class ObstacleManager {
private:
    std::vector<Obstacle> obstacles;
    std::map<ObstacleType, SDL_Texture*> textureMap;
    float timeSinceLastSpawn;
    float baseSpawnInterval;
    float minSpawnInterval;
    float difficulty;
    float getRandomSpawnInterval() const;
    ObstacleType getRandomObstacleType() const;
    float getRandomSpeed() const;
    int getNumberToSpawn() const;
    int getRandomXOffset(int index) const;
    SDL_Rect adjustCollisionBox(const SDL_Rect& obsBox, ObstacleType type) const;

public:
    ObstacleManager(const std::map<ObstacleType, SDL_Texture*>& textures);
    ~ObstacleManager();
    void update(float deltaTime);
    void spawn();
    const std::vector<Obstacle>& getObstacles() const { return obstacles; }
    void clear() { obstacles.clear(); }
    void setDifficulty(float difficulty) { this->difficulty = difficulty; }
    float getDifficulty() const { return difficulty; }

    // Thay đổi kiểu tham số để nhận hình tròn thay vì SDL_Rect
    bool checkCollision(int centerX, int centerY, int radius) const;

    void renderDebugCollision(Graphics* graphics) const;
};

#endif
