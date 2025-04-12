#include "obstacle.h"
#include <algorithm>
#include <cstdlib>

ObstacleManager::ObstacleManager(const std::map<ObstacleType, SDL_Texture*>& textures)
    : textureMap(textures), timeSinceLastSpawn(0.0f),
      baseSpawnInterval(3.0f), minSpawnInterval(1.5f), difficulty(0.0f) {
    // Khởi tạo seed cho random
    srand(static_cast<unsigned int>(time(nullptr)));
}

ObstacleManager::~ObstacleManager() {
    // Dọn dẹp nếu cần
    obstacles.clear();
}

void ObstacleManager::update(float deltaTime) {
    // Cập nhật thời gian từ lần spawn cuối
    timeSinceLastSpawn += deltaTime;

    // Kiểm tra nếu đã đến lúc sinh obstacle mới
    if (timeSinceLastSpawn >= getRandomSpawnInterval()) {
        spawn();
        timeSinceLastSpawn = 0.0f;
    }

    // Cập nhật vị trí của các obstacle
    float baseSpeed = 10.0f + difficulty * 10.0f; // Tăng tốc độ theo độ khó

    for (auto it = obstacles.begin(); it != obstacles.end();) {
        it->move(baseSpeed);

        if (it->isOffScreen()) {
            it = obstacles.erase(it);
        } else {
            ++it;
        }
    }

    // Tăng dần độ khó theo thời gian
    difficulty = std::min(difficulty + deltaTime * 0.002f, 1.0f);
}

float ObstacleManager::getRandomSpawnInterval() const {
    // Khoảng thời gian giữa các lần sinh giảm theo độ khó
    float interval = baseSpawnInterval - (baseSpawnInterval - minSpawnInterval) * difficulty;

    // Thêm một chút ngẫu nhiên
    float randomFactor = (static_cast<float>(rand()) / RAND_MAX) * 1.0f - 0.5f;
    return std::max(interval + randomFactor, minSpawnInterval);
}

ObstacleType ObstacleManager::getRandomObstacleType() const {
    return static_cast<ObstacleType>(rand() % 3);
}

float ObstacleManager::getRandomSpeed() const {
    // Tốc độ cơ bản là 1.0, với điều chỉnh ngẫu nhiên
    return 1.0f + (static_cast<float>(rand()) / RAND_MAX) * 0.2f - 0.1f;
}

int ObstacleManager::getNumberToSpawn() const {
    // Số lượng chướng ngại vật tăng theo độ khó
    float maxObstacles = 1.0f + difficulty * 2.0f;
    return std::min(1 + rand() % static_cast<int>(maxObstacles), 3);
}

int ObstacleManager::getRandomXOffset(int index) const {
    // Khoảng cách giữa các chướng ngại vật liên tiếp
    int baseOffset = 300;
    int randomVariation = rand() % 100 - 50; // ±50 pixel
    return index * (baseOffset + randomVariation);
}

int ObstacleManager::getYPositionForType(ObstacleType type) const {
    // Các loại chướng ngại vật có thể có vị trí Y khác nhau
    switch (type) {
        case ObstacleType::ROCK:
            return 745; // Mặc định
        case ObstacleType::SPIKE:
            return 770; // Cao hơn một chút
        case ObstacleType::LOG:
            return 745; // Mặc định
        default:
            return 745;
    }
}

void ObstacleManager::spawn() {
    int numToSpawn = getNumberToSpawn();

    for (int i = 0; i < numToSpawn; ++i) {
        Obstacle obs;
        ObstacleType type = getRandomObstacleType();

        obs.type = type;
        obs.texture = textureMap.at(type);
        obs.speed = getRandomSpeed();

        // Vị trí X bắt đầu từ bên phải màn hình + offset
        obs.rect.x = SCREEN_WIDTH + getRandomXOffset(i);
        obs.rect.y = getYPositionForType(type);

        // Thiết lập kích thước dựa trên loại obstacle
        switch (type) {
            case ObstacleType::ROCK:
                obs.rect.w = 213;
                obs.rect.h = 264;
                break;
            case ObstacleType::SPIKE:
                obs.rect.w = 100;
                obs.rect.h = 120;
                break;
            case ObstacleType::LOG:
                obs.rect.w = 449;
                obs.rect.h = 177;
                break;
        }

        obstacles.push_back(obs);
    }
}
