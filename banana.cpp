#include "banana.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <set>

Banana::Banana(SDL_Texture* tex, BananaType t, int x, int y) {
    texture = tex;
    type = t;
    active = true;

    // Lấy kích thước gốc của texture
    int width, height;
    if (SDL_QueryTexture(texture, NULL, NULL, &width, &height) != 0) {
        SDL_Log("Failed to query texture dimensions: %s", SDL_GetError());
        width = 50; // Kích thước mặc định nếu thất bại
        height = 50;
    }
    rect.w = width;  // Sử dụng kích thước gốc của ảnh
    rect.h = height;
    value = 1;
    rect.x = x;
    rect.y = y;
}

BananaManager::BananaManager(std::map<BananaType, SDL_Texture*> textures, const PlatformManager& platformMgr, const ObstacleManager& obstacleMgr, Graphics& graphics)
    : platformManager(platformMgr), obstacleManager(obstacleMgr), graphics(graphics) {
    bananaTextures = textures;
    scrollSpeed = 4.0f;
    spawnTimer = 0.0f;
    spawnInterval = 2.0f;
    difficultyFactor = 1.0f;
    srand(static_cast<unsigned>(time(nullptr)));
    lastSpawnTime = 0.0f;
}

BananaManager::~BananaManager() {
    bananas.clear();
    spawnedPlatforms.clear();
    collectedPositions.clear();
}

void BananaManager::update(float deltaTime) {
    spawnTimer += deltaTime;

    // Update banana positions with scrolling
    for (auto it = bananas.begin(); it != bananas.end();) {
        it->rect.x -= static_cast<int>(scrollSpeed);
        if (!it->active || it->rect.x + it->rect.w < -100) {
            it = bananas.erase(it);
        } else {
            // Kiểm tra va chạm với chướng ngại vật
            if (checkObstacleCollision(it->rect, obstacleManager)) {
                it->active = false;
                it = bananas.erase(it); // Xóa chuối nếu nó giao với chướng ngại vật
            } else {
                ++it;
            }
        }
    }

    // Clean up spawnedPlatforms
    for (auto it = spawnedPlatforms.begin(); it != spawnedPlatforms.end();) {
        bool platformExists = false;
        for (const auto& platform : platformManager.getPlatforms()) {
            if (platform.rect.x == *it && platform.active && platform.rect.x + platform.rect.w > -100) {
                platformExists = true;
                break;
            }
        }
        if (!platformExists) {
            it = spawnedPlatforms.erase(it);
        } else {
            ++it;
        }
    }

    // Clean up collectedPositions
    collectedPositions.erase(
        std::remove_if(collectedPositions.begin(), collectedPositions.end(),
            [](const SDL_Point& pos) { return pos.x < -100; }),
        collectedPositions.end()
    );

    // Spawn bananas when timer exceeds interval
    if (spawnTimer - lastSpawnTime >= spawnInterval) {
        spawnBananas();
        lastSpawnTime = spawnTimer;
    }
}

void BananaManager::render() {
    for (const auto& banana : bananas) {
        if (banana.active && banana.rect.x >= -50 && banana.rect.x <= SCREEN_WIDTH) {
            graphics.renderTexture(banana.texture, banana.rect.x, banana.rect.y);
        }
    }
}

bool BananaManager::checkCollision(int playerX, int playerY, int playerRadius, int& score, bool& magnetActivated) {
    bool collected = false;
    magnetActivated = false;

    for (auto it = bananas.begin(); it != bananas.end();) {
        if (!it->active) {
            ++it;
            continue;
        }

        int bananaX = it->rect.x + it->rect.w / 2;
        int bananaY = it->rect.y + it->rect.h / 2;
        int distX = playerX - bananaX;
        int distY = playerY - bananaY;
        float distance = sqrt(distX * distX + distY * distY);

        if (distance <= (playerRadius + it->rect.w / 2)) {
            score += it->value;
            collected = true;

            // Store collected banana position
            SDL_Point pos = {bananaX, bananaY};
            collectedPositions.push_back(pos);

            it->active = false;
            it = bananas.erase(it);
        } else {
            ++it;
        }
    }

    return collected;
}

void BananaManager::spawnBananas() {
    std::vector<Platform> sortedPlatforms;
    const auto& platforms = platformManager.getPlatforms();

    // Find furthest platform edge
    int furthestX = SCREEN_WIDTH;
    for (const auto& platform : platforms) {
        if (platform.active) {
            int platformRight = platform.rect.x + platform.rect.w;
            if (platformRight > furthestX) {
                furthestX = platformRight;
            }
            sortedPlatforms.push_back(platform);
        }
    }
    std::sort(sortedPlatforms.begin(), sortedPlatforms.end(),
              [](const Platform& a, const Platform& b) { return a.rect.x < b.rect.x; });

    // Spawn bananas on platforms beyond SCREEN_WIDTH
    for (const auto& platform : sortedPlatforms) {
        if (platform.rect.x >= SCREEN_WIDTH - 700 && spawnedPlatforms.find(platform.rect.x) == spawnedPlatforms.end()) {
            spawnOnPlatform(platform);
            spawnedPlatforms.insert(platform.rect.x);
        }
    }

    // Spawn bananas on ground beyond furthest platform
    int spawnStart = furthestX;
    int spawnEnd = spawnStart + 1000; // Default value if no next platform found

    // Find the next platform after furthestX
    for (const auto& platform : sortedPlatforms) {
        if (platform.rect.x > spawnStart) {
            spawnEnd = platform.rect.x;
            break;
        }
    }

    // Spawn two clusters on ground with 400-700px gap
    if (spawnStart < SCREEN_WIDTH + 1000) {
        int clusterGap = 400 + (rand() % 301); // Random gap between 400-700px
        int availableSpace = spawnEnd - spawnStart;

        // Lấy kích thước gốc của texture chuối để tính toán khoảng cách
        int bananaWidth, bananaHeight;
        if (SDL_QueryTexture(bananaTextures.at(BananaType::NORMAL), NULL, NULL, &bananaWidth, &bananaHeight) != 0) {
            SDL_Log("Failed to query banana texture dimensions: %s", SDL_GetError());
            bananaWidth = 50; // Kích thước mặc định nếu thất bại
            bananaHeight = 50;
        }
        int clusterWidth = (bananaWidth + 10) * 3; // Ước lượng chiều rộng cụm (3 chuối ngang, với khoảng cách)
        int totalWidth = 2 * clusterWidth + clusterGap;

        if (availableSpace > totalWidth) {
            int firstClusterStart = spawnStart + (availableSpace - totalWidth) / 2;
            int secondClusterStart = firstClusterStart + clusterWidth + clusterGap;

            spawnOnGround(firstClusterStart, firstClusterStart + clusterWidth);
            spawnOnGround(secondClusterStart, secondClusterStart + clusterWidth);
        }
    }
}

void BananaManager::spawnOnPlatform(const Platform& platform) {
    // Lấy kích thước gốc của texture chuối
    int bananaWidth, bananaHeight;
    if (SDL_QueryTexture(bananaTextures.at(BananaType::NORMAL), NULL, NULL, &bananaWidth, &bananaHeight) != 0) {
        SDL_Log("Failed to query banana texture dimensions: %s", SDL_GetError());
        bananaWidth = 50; // Kích thước mặc định nếu thất bại
        bananaHeight = 50;
    }
    int spacing = bananaWidth + 10; // Khoảng cách giữa các quả chuối, dựa trên kích thước gốc

    // Chọn mẫu ngẫu nhiên: 0 = dọc (2 quả), 1 = ngang (3 quả)
    int patternType = rand() % 2;

    if (patternType == 0) {
        // Mẫu dọc: 2 quả chuối
        int posX = platform.rect.x + (platform.rect.w - bananaWidth) / 2; // Căn giữa platform
        int posY = platform.rect.y - bananaHeight - 10; // Đặt chuối phía trên platform

        // Kiểm tra và sinh quả chuối dưới
        SDL_Rect bananaRect = {posX, posY, bananaWidth, bananaHeight};
        if (!isNearCollectedPosition(posX, posY) && isValidBananaPosition(posX, posY, bananaWidth, bananaHeight) && !checkObstacleCollision(bananaRect, obstacleManager)) {
            Banana newBanana(bananaTextures.at(BananaType::NORMAL), BananaType::NORMAL, posX, posY);
            bananas.push_back(newBanana);
        }

        // Kiểm tra và sinh quả chuối trên
        posY -= bananaHeight + 10;
        bananaRect = {posX, posY, bananaWidth, bananaHeight};
        if (!isNearCollectedPosition(posX, posY) && isValidBananaPosition(posX, posY, bananaWidth, bananaHeight) && !checkObstacleCollision(bananaRect, obstacleManager)) {
            Banana newBanana(bananaTextures.at(BananaType::NORMAL), BananaType::NORMAL, posX, posY);
            bananas.push_back(newBanana);
        }
    } else {
        // Mẫu ngang: 3 quả chuối
        int maxBananas = std::min(3, static_cast<int>(platform.rect.w / spacing));
        if (maxBananas < 1) maxBananas = 1;

        int startX = platform.rect.x + (platform.rect.w - maxBananas * spacing) / 2;
        int posY = platform.rect.y - bananaHeight - 10;

        for (int i = 0; i < maxBananas; ++i) {
            int posX = startX + i * spacing;
            SDL_Rect bananaRect = {posX, posY, bananaWidth, bananaHeight};
            if (!isNearCollectedPosition(posX, posY) && isValidBananaPosition(posX, posY, bananaWidth, bananaHeight) && !checkObstacleCollision(bananaRect, obstacleManager)) {
                Banana newBanana(bananaTextures.at(BananaType::NORMAL), BananaType::NORMAL, posX, posY);
                bananas.push_back(newBanana);
            }
        }
    }
}

void BananaManager::spawnOnGround(int startX, int endX) {
    // Lấy kích thước gốc của texture chuối
    int bananaWidth, bananaHeight;
    if (SDL_QueryTexture(bananaTextures.at(BananaType::NORMAL), NULL, NULL, &bananaWidth, &bananaHeight) != 0) {
        SDL_Log("Failed to query banana texture dimensions: %s", SDL_GetError());
        bananaWidth = 50; // Kích thước mặc định nếu thất bại
        bananaHeight = 50;
    }
    int spacing = bananaWidth + 10; // Khoảng cách giữa các quả chuối, dựa trên kích thước gốc

    // Chọn mẫu ngẫu nhiên: 0 = dọc (2 quả), 1 = ngang (3 quả)
    int patternType = rand() % 2;

    if (patternType == 0) {
        // Mẫu dọc: 2 quả chuối
        int posX = startX + (endX - startX - bananaWidth) / 2; // Căn giữa cụm
        int posY = GROUND_LEVEL - bananaHeight - 10;

        // Kiểm tra và sinh quả chuối dưới
        SDL_Rect bananaRect = {posX, posY, bananaWidth, bananaHeight};
        if (!isNearCollectedPosition(posX, posY) && isValidBananaPosition(posX, posY, bananaWidth, bananaHeight) && !checkObstacleCollision(bananaRect, obstacleManager)) {
            Banana newBanana(bananaTextures.at(BananaType::NORMAL), BananaType::NORMAL, posX, posY);
            bananas.push_back(newBanana);
        }

        // Kiểm tra và sinh quả chuối trên
        posY -= bananaHeight + 10;
        bananaRect = {posX, posY, bananaWidth, bananaHeight};
        if (!isNearCollectedPosition(posX, posY) && isValidBananaPosition(posX, posY, bananaWidth, bananaHeight) && !checkObstacleCollision(bananaRect, obstacleManager)) {
            Banana newBanana(bananaTextures.at(BananaType::NORMAL), BananaType::NORMAL, posX, posY);
            bananas.push_back(newBanana);
        }
    } else {
        // Mẫu ngang: 3 quả chuối
        int maxBananas = 3;
        int startXAdjusted = startX + (endX - startX - maxBananas * spacing) / 2;
        int posY = GROUND_LEVEL - bananaHeight - 10;

        for (int i = 0; i < maxBananas; ++i) {
            int posX = startXAdjusted + i * spacing;
            SDL_Rect bananaRect = {posX, posY, bananaWidth, bananaHeight};
            if (!isNearCollectedPosition(posX, posY) && isValidBananaPosition(posX, posY, bananaWidth, bananaHeight) && !checkObstacleCollision(bananaRect, obstacleManager)) {
                Banana newBanana(bananaTextures.at(BananaType::NORMAL), BananaType::NORMAL, posX, posY);
                bananas.push_back(newBanana);
            }
        }
    }
}

bool BananaManager::checkObstacleCollision(const SDL_Rect& bananaRect, const ObstacleManager& obstacleMgr) const {
    for (const auto& obs : obstacleMgr.getObstacles()) {
        // Lấy khung va chạm thực tế từ ObstacleManager
        SDL_Rect collisionBox = obstacleMgr.adjustCollisionBox(obs.rect, obs.type);

        // Mở rộng khung va chạm thêm vùng an toàn
        SDL_Rect expandedObsRect = collisionBox;
        expandedObsRect.x -= 50; // Vùng an toàn 50 pixel
        expandedObsRect.y -= 50;
        expandedObsRect.w += 100; // 50 * 2
        expandedObsRect.h += 100;
        if (SDL_HasIntersection(&bananaRect, &expandedObsRect)) {
            return true;
        }
    }
    return false;
}

bool BananaManager::isValidBananaPosition(int x, int y, int width, int height) const {
    if (y < 0 || y + height > GROUND_LEVEL) {
        return false;
    }

    int thisCenterX = x + width / 2;
    int thisCenterY = y + height / 2;
    const int minDistance = 40; // Giảm khoảng cách tối thiểu để chuối có thể gần nhau hơn

    for (const auto& banana : bananas) {
        int otherCenterX = banana.rect.x + banana.rect.w / 2;
        int otherCenterY = banana.rect.y + banana.rect.h / 2;
        int distX = thisCenterX - otherCenterX;
        int distY = thisCenterY - otherCenterY;
        float distance = sqrt(distX * distX + distY * distY);
        if (distance < minDistance) {
            return false;
        }
    }

    return true;
}

bool BananaManager::isNearCollectedPosition(int x, int y) const {
    // Lấy kích thước gốc của texture chuối để tính toán tâm
    int bananaWidth, bananaHeight;
    if (SDL_QueryTexture(bananaTextures.at(BananaType::NORMAL), NULL, NULL, &bananaWidth, &bananaHeight) != 0) {
        SDL_Log("Failed to query banana texture dimensions: %s", SDL_GetError());
        bananaWidth = 50; // Kích thước mặc định nếu thất bại
        bananaHeight = 50;
    }
    int thisCenterX = x + bananaWidth / 2;
    int thisCenterY = y + bananaHeight / 2;
    const int minDistance = 50; // Giảm khoảng cách để chuối có thể xuất hiện gần vị trí đã thu thập hơn

    for (const auto& pos : collectedPositions) {
        if (pos.x < 0 || pos.x > SCREEN_WIDTH) continue;
        int distX = thisCenterX - pos.x;
        int distY = thisCenterY - pos.y;
        float distance = sqrt(distX * distX + distY * distY);
        if (distance < minDistance) {
            return true;
        }
    }
    return false;
}

void BananaManager::setScrollSpeed(float speed) {
    scrollSpeed = speed;
}

void BananaManager::setDifficultyFactor(float difficulty) {
    difficultyFactor = difficulty;
    spawnInterval = 2.0f - (difficulty - 1.0f) * 0.15f;
    if (spawnInterval < 0.5f) spawnInterval = 0.5f;
}

void BananaManager::clear() {
    bananas.clear();
    spawnedPlatforms.clear();
    collectedPositions.clear();
}

void BananaManager::renderDebugCollision(SDL_Renderer* renderer) {
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);

    for (const auto& banana : bananas) {
        if (banana.active && banana.rect.x >= -50 && banana.rect.x <= SCREEN_WIDTH) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 128);
            SDL_Rect outlineRect = banana.rect;
            SDL_RenderDrawRect(renderer, &outlineRect);
        }
    }

    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}
