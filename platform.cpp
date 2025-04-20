#include "platform.h"
#include "player.h"

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
            rect.w = 867;
            rect.h = 155;
            break;
        case PlatformType::LAND_SMALL:
            rect.w = 750;
            rect.h = 159;
            break;
        case PlatformType::VINE:
            rect.w = 697;
            rect.h = 487;
            break;
    }

    rect.x = x;
    rect.y = y;
}

PlatformManager::PlatformManager(std::map<PlatformType, SDL_Texture*> textures) {
    platformTextures = textures;
    scrollSpeed = 5.0f; // Tốc độ mặc định
    spawnDelay = 9000; // 3 giây giữa mỗi lần sinh platform
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

// Kiểm tra xem tại vị trí (x, y) có nền đất không
bool PlatformManager::hasLandPlatformAt(int x, int y, int tolerance) const {
    for (const auto& platform : platforms) {
        if (platform.active &&
            (platform.type == PlatformType::LAND_MID || platform.type == PlatformType::LAND_SMALL)) {
            // Kiểm tra xem điểm (x, y) có nằm trên platform không
            if (x >= platform.rect.x && x <= platform.rect.x + platform.rect.w &&
                y >= platform.rect.y - tolerance && y <= platform.rect.y + tolerance) {
                return true;
            }
        }
    }
    return false;
}

// Kiểm tra xem tại vị trí (x, y) có nền cỏ không
bool PlatformManager::hasGrassPlatformAt(int x, int y, int tolerance) const {
    for (const auto& platform : platforms) {
        if (platform.active &&
            (platform.type == PlatformType::GRASS_BIG ||
             platform.type == PlatformType::GRASS_MID ||
             platform.type == PlatformType::GRASS_SUPERBIG)) {
            // Kiểm tra xem điểm (x, y) có nằm trên platform không
            if (x >= platform.rect.x && x <= platform.rect.x + platform.rect.w &&
                y >= platform.rect.y - tolerance && y <= platform.rect.y + tolerance) {
                return true;
            }
        }
    }
    return false;
}

// Tìm vị trí y hợp lệ cho nền cỏ theo logic
int PlatformManager::findValidYForGrass(int x, int kongHeight) const {
    const int GROUND_LEVEL = 730; // Mực nước biển/mặt đất
    vector<int> validPositions;

    // Trường hợp 1: Cao hơn nhân vật khỉ so với mặt đất
    validPositions.push_back(GROUND_LEVEL);

    // Trường hợp 2: Cao hơn nhân vật khỉ + nền đất
    for (const auto& platform : platforms) {
        if (platform.active &&
            (platform.type == PlatformType::LAND_MID || platform.type == PlatformType::LAND_SMALL)) {
            // Kiểm tra xem platform đất có gần vị trí x không
            if (x >= platform.rect.x - 20 && x <= platform.rect.x + platform.rect.w + 20) {
                validPositions.push_back(platform.rect.y - kongHeight);
            }
        }
    }

    // Trường hợp 3: Cao hơn nhân vật khỉ + nền cỏ
    for (const auto& platform : platforms) {
        if (platform.active &&
            (platform.type == PlatformType::GRASS_BIG ||
             platform.type == PlatformType::GRASS_MID ||
             platform.type == PlatformType::GRASS_SUPERBIG)) {
            // Kiểm tra xem platform cỏ có gần vị trí x không
            if (x >= platform.rect.x - 20 && x <= platform.rect.x + platform.rect.w + 20) {
                // Trường hợp 3a: Cao hơn khỉ + cỏ + khỉ
                validPositions.push_back(platform.rect.y - kongHeight - kongHeight);

                // Trường hợp 3b: Cao hơn land + khỉ + cỏ + khỉ
                // Kiểm tra nếu có nền đất phía dưới nền cỏ
                for (const auto& landPlatform : platforms) {
                    if (landPlatform.active &&
                        (landPlatform.type == PlatformType::LAND_MID || landPlatform.type == PlatformType::LAND_SMALL)) {
                        if (platform.rect.y == landPlatform.rect.y - kongHeight - platform.rect.h) {
                            validPositions.push_back(platform.rect.y - kongHeight - kongHeight);
                        }
                    }
                }
            }
        }
    }

    // Nếu không tìm thấy vị trí hợp lệ, sử dụng vị trí mặc định
    if (validPositions.empty()) {
        return GROUND_LEVEL; // Vị trí mặc định nếu không tìm thấy vị trí hợp lệ
    }

    // Chọn ngẫu nhiên một trong các vị trí hợp lệ
    return validPositions[rand() % validPositions.size()];
}

// Kiểm tra vị trí hợp lệ cho chướng ngại vật
bool PlatformManager::isValidPositionForObstacle(int x, int y, int width, int height) const {
    const int GROUND_LEVEL = 730; // Mực nước biển/mặt đất

    // Kiểm tra vị trí trên mặt đất
    if (y + height == GROUND_LEVEL) {
        return true;
    }

    // Kiểm tra vị trí trên nền cỏ
    for (const auto& platform : platforms) {
        if (platform.active &&
            (platform.type == PlatformType::GRASS_BIG ||
             platform.type == PlatformType::GRASS_MID ||
             platform.type == PlatformType::GRASS_SUPERBIG)) {
            // Kiểm tra xem chướng ngại vật có nằm hoàn toàn trên platform không
            if (x >= platform.rect.x && x + width <= platform.rect.x + platform.rect.w &&
                y + height == platform.rect.y) {
                return true;
            }
        }
    }

    // Kiểm tra vị trí trên nền đất
    for (const auto& platform : platforms) {
        if (platform.active &&
            (platform.type == PlatformType::LAND_MID || platform.type == PlatformType::LAND_SMALL)) {
            // Kiểm tra xem chướng ngại vật có nằm hoàn toàn trên platform không
            if (x >= platform.rect.x && x + width <= platform.rect.x + platform.rect.w &&
                y + height == platform.rect.y) {
                return true;
            }
        }
    }

    // Kiểm tra vị trí trên mặt đất đằng sau nền đất
    for (const auto& platform : platforms) {
        if (platform.active &&
            (platform.type == PlatformType::LAND_MID || platform.type == PlatformType::LAND_SMALL)) {
            // Kiểm tra xem chướng ngại vật có nằm trước platform không
            if (x < platform.rect.x && x + width >= platform.rect.x &&
                y + height == GROUND_LEVEL) {
                return true;
            }
        }
    }

    return false;
}

void PlatformManager::spawnPlatform() {
    const int GROUND_LEVEL = 730; // Mực nước biển/mặt đất
    const int KONG_HEIGHT = 149; // Chiều cao của nhân vật khỉ

    // Xác định các loại platform có thể được tạo
    std::vector<PlatformType> grassTypes = {
        PlatformType::GRASS_BIG, PlatformType::GRASS_MID, PlatformType::GRASS_SUPERBIG
    };

    std::vector<PlatformType> landTypes = {
        PlatformType::LAND_MID, PlatformType::LAND_SMALL
    };

    // Chọn ngẫu nhiên giữa nền đất và nền cỏ (80% cỏ, 20% đất)
    bool spawnGrass = (rand() % 100) < 50; //80

    // Chọn ngẫu nhiên một loại platform
    PlatformType randomType;
    if (spawnGrass) {
        randomType = grassTypes[rand() % grassTypes.size()];
    } else {
        randomType = landTypes[rand() % landTypes.size()];
    }

    // Kiểm tra xem có texture tương ứng không
    if (platformTextures.find(randomType) != platformTextures.end()) {
        int xPos = SCREEN_WIDTH; // Bắt đầu từ mép phải màn hình
        int yPos;

        // Tính toán vị trí y tùy thuộc vào loại platform
        if (randomType == PlatformType::LAND_MID || randomType == PlatformType::LAND_SMALL) {
            yPos = GROUND_LEVEL; // Land platform nằm trên mặt đất
        } else {
            // Tìm vị trí y hợp lệ cho nền cỏ theo logic
            yPos = findValidYForGrass(xPos, KONG_HEIGHT);
        }

        // Tạo platform mới
        Platform newPlatform(platformTextures[randomType], randomType, xPos, yPos);

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
