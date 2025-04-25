#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <SDL.h>
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
    bool isPlatform;
    float speed;
};

class ObstacleManager {
public:
    ObstacleManager(const std::map<ObstacleType, SDL_Texture*>& textures);
    ~ObstacleManager();

    void update(float deltaTime, float scrollSpeed);
    void setDifficulty(float diff);
    bool checkCollision(int centerX, int centerY, int radius) const;
    void renderDebugCollision(Graphics* graphics) const;
    const std::vector<Obstacle>& getObstacles() const;
    void clear();

    void addObstacle(const Obstacle& obstacle);
    SDL_Texture* getTextureForType(ObstacleType type) const;
    ObstacleType getRandomObstacleType() const;

private:
    std::vector<Obstacle> obstacles;
    std::map<ObstacleType, SDL_Texture*> textureMap;
    float difficulty;
    SDL_Rect adjustCollisionBox(const SDL_Rect& obsBox, ObstacleType type) const;
};

#endif
