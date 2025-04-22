#ifndef OBSTACLE_H
#define OBSTACLE_H
#include <vector>
#include <map>
#include "graphics.h"

enum class ObstacleType {
    ROCK,
    SPIKE,
    PLANE
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
    float difficulty;
    SDL_Rect adjustCollisionBox(const SDL_Rect& obsBox, ObstacleType type) const;

public:
    ObstacleManager(const std::map<ObstacleType, SDL_Texture*>& textures);
    ~ObstacleManager();
    void update(float deltaTime);
    const std::vector<Obstacle>& getObstacles() const { return obstacles; }
    void clear() { obstacles.clear(); }
    void setDifficulty(float difficulty) { this->difficulty = difficulty; }
    float getDifficulty() const { return difficulty; }

    bool checkCollision(int centerX, int centerY, int radius) const;
    void renderDebugCollision(Graphics* graphics) const;

    ObstacleType getRandomObstacleType() const;
    float getRandomSpeed() const;
    SDL_Texture* getTextureForType(ObstacleType type) const {
        auto it = textureMap.find(type);
        return (it != textureMap.end()) ? it->second : nullptr;
    }
    void addObstacle(const Obstacle& obstacle) {
        obstacles.push_back(obstacle);
    }
};

#endif //OBSTACLE__H_
