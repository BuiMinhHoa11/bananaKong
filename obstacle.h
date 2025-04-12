#ifndef OBSTACLE_H_
#define OBSTACLE_H_

#include "common_func.h"
#include <vector>
#include <map>

enum class ObstacleType {
    ROCK,
    SPIKE,
    LOG
};

struct Obstacle {
    SDL_Rect rect;
    SDL_Texture* texture;
    ObstacleType type;
    float speed; // Tốc độ di chuyển của từng chướng ngại vật

    void move(float baseSpeed) {
        rect.x -= static_cast<int>(baseSpeed * speed);
    }

    bool isOffScreen() const {
        return rect.x + rect.w < 0;
    }
};

// Lớp quản lý chướng ngại vật
class ObstacleManager {
public:
    ObstacleManager(const std::map<ObstacleType, SDL_Texture*>& textures);
    ~ObstacleManager();

    void update(float deltaTime);
    void spawn();
    const std::vector<Obstacle>& getObstacles() const { return obstacles; }
    void clear() { obstacles.clear(); }
    void setDifficulty(float difficulty) { this->difficulty = difficulty; }
    float getDifficulty() const { return difficulty; }

private:
    std::vector<Obstacle> obstacles;
    std::map<ObstacleType, SDL_Texture*> textureMap;
    float timeSinceLastSpawn;
    float baseSpawnInterval;
    float minSpawnInterval;
    float difficulty; // 0.0 - 1.0, ảnh hưởng đến tần suất, tốc độ và số lượng

    float getRandomSpawnInterval() const;
    ObstacleType getRandomObstacleType() const;
    float getRandomSpeed() const;
    int getNumberToSpawn() const;
    int getRandomXOffset(int index) const;
    int getYPositionForType(ObstacleType type) const;
};

#endif // OBSTACLE_H_
