#include "obstacle.h"
#include "platform.h"

ObstacleManager::ObstacleManager(const std::map<ObstacleType, SDL_Texture*>& textures) {
    textureMap = textures;
    timeSinceLastSpawn = 0.0f;
    baseSpawnInterval = 7.0f;  // Khoảng thời gian cơ bản giữa các lần sinh
    minSpawnInterval = 1.0f;   // Khoảng thời gian tối thiểu giữa các lần sinh
    difficulty = 1.0f;         // Độ khó ban đầu
}

ObstacleManager::~ObstacleManager() {
    // Xóa bỏ tất cả chướng ngại vật khi hủy manager
    obstacles.clear();
}

void ObstacleManager::update(float deltaTime) {
    // Cập nhật thời gian
    timeSinceLastSpawn += deltaTime;

    // Kiểm tra xem đã đến lúc sinh chướng ngại vật chưa
    if (timeSinceLastSpawn >= getRandomSpawnInterval()) {
        spawn();
        timeSinceLastSpawn = 0.0f;
    }

    // Cập nhật vị trí của tất cả chướng ngại vật
    for (auto it = obstacles.begin(); it != obstacles.end();) {
        // Di chuyển chướng ngại vật
        it->rect.x -= static_cast<int>(5.0f * it->speed); // Sử dụng tốc độ di chuyển để tính toán vị trí mới

        // Nếu chướng ngại vật ra khỏi màn hình, xóa nó
        if (it->rect.x + it->rect.w < 0) {
            it = obstacles.erase(it);
        } else {
            ++it;
        }
    }
}

void ObstacleManager::spawn() {
    // Chọn loại chướng ngại vật ngẫu nhiên
    ObstacleType type = getRandomObstacleType();

    // Kiểm tra xem có texture tương ứng không
    if (textureMap.find(type) == textureMap.end()) {
        return;
    }

    // Thiết lập kích thước dựa trên loại chướng ngại vật
    SDL_Rect rect;
    switch (type) {
            case ObstacleType::ROCK:
                rect.w = 209;
                rect.h = 133;
                break;

            case ObstacleType::SPIKE:
                rect.w = 106;
                rect.h = 133;
                break;

            case ObstacleType::PLANE:
                rect.w = 270;
                rect.h = 133;
                break;
    }

    // Thiết lập vị trí x ban đầu (ngoài màn hình bên phải)
    rect.x = SCREEN_WIDTH;

    // Thiết lập vị trí y dựa trên logic
    // Nếu có PlatformManager, hãy sử dụng nó để kiểm tra vị trí hợp lệ
    PlatformManager* platformManager = nullptr; // Cần được truyền vào hoặc thiết lập

    // Xác định các vị trí y có thể đặt chướng ngại vật
    const int GROUND_LEVEL = 755; // Mực nước biển/mặt đất
    std::vector<int> possibleYPositions;

    // Vị trí mặc định trên mặt đất
    possibleYPositions.push_back(GROUND_LEVEL);

    // Nếu có platformManager, thêm vị trí trên các nền tảng hiện có
    if (platformManager) {
        const std::vector<Platform>& platforms = platformManager->getPlatforms();
        for (const auto& platform : platforms) {
            if (platform.active) {
                if (platform.type == PlatformType::GRASS_BIG ||
                    platform.type == PlatformType::GRASS_MID ||
                    platform.type == PlatformType::GRASS_SUPERBIG) {
                    possibleYPositions.push_back(platform.rect.y - rect.h);
                } else if (platform.type == PlatformType::LAND_MID ||
                           platform.type == PlatformType::LAND_SMALL) {
                    possibleYPositions.push_back(platform.rect.y - rect.h);
                }
            }
        }
    }

    // Chọn ngẫu nhiên một vị trí y từ các vị trí có thể
    rect.y = possibleYPositions[rand() % possibleYPositions.size()];

    // Tạo chướng ngại vật mới
    Obstacle newObstacle;
    newObstacle.type = type;
    newObstacle.rect = rect;
    newObstacle.texture = textureMap[type];
    newObstacle.speed = getRandomSpeed();
    newObstacle.isPlatform = false; // Đây không phải là platform

    // Thêm chướng ngại vật vào danh sách
    obstacles.push_back(newObstacle);
}

// Kiểm tra va chạm với nhân vật người chơi (sử dụng va chạm hình tròn)
bool ObstacleManager::checkCollision(int centerX, int centerY, int radius) const {
    for (const auto& obstacle : obstacles) {
        // Tính toán hình chữ nhật va chạm điều chỉnh cho chướng ngại vật
        SDL_Rect collisionBox = adjustCollisionBox(obstacle.rect, obstacle.type);

        // Tính toán tâm của hình chữ nhật
        int obsCenterX = collisionBox.x + collisionBox.w / 2;
        int obsCenterY = collisionBox.y + collisionBox.h / 2;

        // Tính khoảng cách giữa tâm hình tròn và tâm hình chữ nhật
        int distX = abs(centerX - obsCenterX);
        int distY = abs(centerY - obsCenterY);

        // Nếu khoảng cách quá xa, không có va chạm
        if (distX > (collisionBox.w / 2 + radius)) continue;
        if (distY > (collisionBox.h / 2 + radius)) continue;

        // Nếu khoảng cách đủ gần, có va chạm
        if (distX <= (collisionBox.w / 2)) return true;
        if (distY <= (collisionBox.h / 2)) return true;

        // Kiểm tra va chạm ở góc
        int cornerDistSq = pow(distX - collisionBox.w / 2, 2) +
                           pow(distY - collisionBox.h / 2, 2);

        if (cornerDistSq <= pow(radius, 2)) return true;
    }

    return false;
}

SDL_Rect ObstacleManager::adjustCollisionBox(const SDL_Rect& obsBox, ObstacleType type) const {
    SDL_Rect adjusted = obsBox;
    switch (type) {
        case ObstacleType::ROCK:
            //adjustedBox.x += 25;
            adjusted.w = 10;
            adjusted.y += 50;
            adjusted.h = 103;
            break;

        case ObstacleType::SPIKE:
            adjusted.x += 16;
            adjusted.w = 10;
            adjusted.y += 23;
            adjusted.h = 90;
            break;

        case ObstacleType::PLANE:
            adjusted.x += 25;
            adjusted.w = 10;
            adjusted.y += 30;
            adjusted.h = 100;
            break;
    }

    return adjusted;
}

void ObstacleManager::renderDebugCollision(Graphics* graphics) const {
    // Lưu màu hiện tại
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(graphics->getRenderer(), &r, &g, &b, &a);

    // Thiết lập màu đỏ cho khung va chạm chướng ngại vật
    SDL_SetRenderDrawColor(graphics->getRenderer(), 255, 0, 0, 128);

    // Vẽ khung va chạm cho tất cả chướng ngại vật
    for (const auto& obstacle : obstacles) {
        // Lấy khung va chạm điều chỉnh
        SDL_Rect collisionBox = adjustCollisionBox(obstacle.rect, obstacle.type);

        // Vẽ khung hình chữ nhật
        SDL_RenderDrawRect(graphics->getRenderer(), &collisionBox);
    }

    // Khôi phục màu ban đầu
    SDL_SetRenderDrawColor(graphics->getRenderer(), r, g, b, a);
}

float ObstacleManager::getRandomSpawnInterval() const {
    float interval = baseSpawnInterval - (difficulty * 0.1f);
    if (interval < minSpawnInterval) {
        interval = minSpawnInterval;
    }

    // Thêm độ ngẫu nhiên vào khoảng thời gian
    return interval + (static_cast<float>(rand()) / RAND_MAX) * interval * 0.5f;
}

ObstacleType ObstacleManager::getRandomObstacleType() const {
    int r = rand() % 3;
    switch (r) {
        case 0: return ObstacleType::ROCK;
        case 1: return ObstacleType::SPIKE;
        default: return ObstacleType::PLANE;
    }
}

float ObstacleManager::getRandomSpeed() const {
    // Tạo tốc độ ngẫu nhiên trong khoảng từ 0.8 đến 1.2
    return 0.8f + (static_cast<float>(rand()) / RAND_MAX) * 0.4f;
}

int ObstacleManager::getNumberToSpawn() const {
    // Tùy thuộc vào độ khó, có thể sinh nhiều chướng ngại vật cùng lúc
    if (difficulty > 3.0f) {
        if (rand() % 100 < 30) return 2; // 30% cơ hội sinh 2 chướng ngại vật
    }

    return 1; // Mặc định sinh 1 chướng ngại vật
}

int ObstacleManager::getRandomXOffset(int index) const {
    // Tạo khoảng cách giữa các chướng ngại vật khi sinh nhiều cùng lúc
    return index * 200 + (rand() % 100);
}
