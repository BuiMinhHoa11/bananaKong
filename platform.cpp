// platform.cpp
#include "platform.h"
#include <algorithm>

Platform::Platform(SDL_Texture* tex, PlatformType t, int x, int y) {
    texture = tex;
    type = t;
    active = true;

    // Thiết lập kích thước dựa trên loại nền tảng
    switch(type) {
        case PlatformType::GRASS_BIG:
            rect.w = 498;
            rect.h = 38;
            break;
        case PlatformType::GRASS_MID:
            rect.w = 282;
            rect.h = 34;
            break;
        case PlatformType::GRASS_SUPERBIG:
            rect.w = 728;
            rect.h = 34;
            break;
        case PlatformType::LAND_MID:
            rect.w = 895;
            rect.h = 160;
            break;
        case PlatformType::LAND_SMALL:
            rect.w = 774;
            rect.h = 160;
            break;
        case PlatformType::VINE:
            rect.w = 697;
            rect.h = 487;
            break;
    }
    rect.x = x;

    // Thiết lập vị trí y dựa trên loại platform
    if (type == PlatformType::VINE) {
        rect.y = 0; // Vine bắt đầu từ trên cùng của màn hình
    } else if (type == PlatformType::LAND_MID || type == PlatformType::LAND_SMALL) {
        rect.y = 725;
    } else {
        // Các loại grass: mép dưới ở y = 755 - 167 (chiều cao Kong)
        rect.y = 500;
    }
}

PlatformManager::PlatformManager(std::map<PlatformType, SDL_Texture*> textures) {
    platformTextures = textures;
    scrollSpeed = 5.0f; // Tốc độ mặc định, có thể điều chỉnh
    spawnDelay = 1500; // 1.5 giây giữa mỗi lần sinh platform
    spawnTimer = 0;
}

void PlatformManager::update(float deltaTime) {
    // Cập nhật tất cả platform hiện có
    for (auto& platform : platforms) {
        // Di chuyển platform từ phải sang trái
        platform.rect.x -= static_cast<int>(scrollSpeed);

        // Đánh dấu platform không còn active nếu đã ra khỏi màn hình
        if (platform.rect.x + platform.rect.w < 0) {
            platform.active = false;
        }
    }

    // Xóa các platform không còn active
    platforms.erase(
        std::remove_if(platforms.begin(), platforms.end(),
            [](const Platform& p) { return !p.active; }),
        platforms.end()
    );

    // Cập nhật bộ đếm thời gian để sinh platform mới
    spawnTimer += static_cast<int>(deltaTime * 1000);
    if (spawnTimer >= spawnDelay) {
        spawnPlatform();
        spawnTimer = 0;
    }
}

void PlatformManager::spawnPlatform() {
    // Chọn ngẫu nhiên loại platform để sinh
    std::vector<PlatformType> types = {
        PlatformType::GRASS_BIG, PlatformType::GRASS_MID,
        PlatformType::GRASS_SUPERBIG, PlatformType::LAND_MID,
        PlatformType::LAND_SMALL, PlatformType::VINE
    };

    PlatformType randomType = types[rand() % types.size()];

    // Chỉ sinh platform nếu có texture tương ứng
    if (platformTextures.find(randomType) != platformTextures.end()) {
        Platform newPlatform(
            platformTextures[randomType],
            randomType,
            SCREEN_WIDTH, // Bắt đầu từ mép phải màn hình
            0 // Y sẽ được tính trong constructor của Platform
        );

        // Kiểm tra khoảng cách với platform cuối cùng để tránh chồng chéo
        bool canSpawn = true;
        if (!platforms.empty()) {
            Platform& lastPlatform = platforms.back();
            if (SCREEN_WIDTH - (lastPlatform.rect.x + lastPlatform.rect.w) < 300) {
                canSpawn = false; // Không đủ khoảng cách
            }
        }

        if (canSpawn) {
            platforms.push_back(newPlatform);
        }
    }
}

void PlatformManager::render(Graphics* graphics) {
    // Render tất cả platform active
    for (const auto& platform : platforms) {
        if (platform.active) {
            graphics->renderTexture(platform.texture, platform.rect.x, platform.rect.y);

            // Nếu muốn hiển thị khung va chạm (debug)
            renderDebugCollision(graphics, platform);
        }
    }
}

void PlatformManager::renderDebugCollision(Graphics* graphics, const Platform& platform) {
    // Lưu màu hiện tại
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(graphics->getRenderer(), &r, &g, &b, &a);

    // Thiết lập màu xanh lá cho khung va chạm platform
    SDL_SetRenderDrawColor(graphics->getRenderer(), 0, 255, 0, 128);

    // Vẽ khung hình chữ nhật cho platform
    SDL_Rect outlineRect = platform.rect;
    SDL_RenderDrawRect(graphics->getRenderer(), &outlineRect);

    // Khôi phục màu ban đầu
    SDL_SetRenderDrawColor(graphics->getRenderer(), r, g, b, a);
}

const std::vector<Platform>& PlatformManager::getPlatforms() const {
    return platforms;
}

void PlatformManager::setScrollSpeed(float speed) {
    scrollSpeed = speed;
}

void PlatformManager::clear() {
    platforms.clear();
}
