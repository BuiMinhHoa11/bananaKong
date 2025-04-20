/*// platform.h
#ifndef PLATFORM_H_
#define PLATFORM_H_

#include "common_func.h"
#include "graphics.h"
#include <vector>

enum class PlatformType {
    GRASS_BIG,       // 538x80
    GRASS_MID,       // 310x80
    GRASS_SUPERBIG,  // 1003x80
    LAND_MID,        // 898x176
    LAND_SMALL,      // 827x176
    VINE             // 697x500
};

struct Platform {
    SDL_Rect rect;
    SDL_Texture* texture;
    PlatformType type;
    bool active; // Để biết nền tảng có hiển thị trên màn hình không

    Platform(SDL_Texture* tex, PlatformType t, int x, int y) {
        texture = tex;
        type = t;
        active = true;

        // Thiết lập kích thước dựa trên loại nền tảng
        switch(type) {
            case PlatformType::GRASS_BIG:
                rect.w = 538;
                rect.h = 80;
                break;
            case PlatformType::GRASS_MID:
                rect.w = 310;
                rect.h = 80;
                break;
            case PlatformType::GRASS_SUPERBIG:
                rect.w = 1003;
                rect.h = 80;
                break;
            case PlatformType::LAND_MID:
                rect.w = 898;
                rect.h = 176;
                break;
            case PlatformType::LAND_SMALL:
                rect.w = 827;
                rect.h = 176;
                break;
            case PlatformType::VINE:
                rect.w = 697;
                rect.h = 500;
                break;
        }

        rect.x = x;

        // Thiết lập vị trí y dựa trên loại platform
        if (type == PlatformType::VINE) {
            rect.y = 0; // Vine bắt đầu từ trên cùng của màn hình
        } else if (type == PlatformType::LAND_MID || type == PlatformType::LAND_SMALL) {
            rect.y = 755 - rect.h; // Mép dưới ở y=755
        } else {
            // Các loại grass: mép dưới ở y = 755 - 167 (chiều cao Kong)
            rect.y = 755 - 167 - rect.h;
        }
    }
};

class PlatformManager {
private:
    std::map<PlatformType, SDL_Texture*> platformTextures;
    std::vector<Platform> platforms;
    float scrollSpeed; // Tốc độ di chuyển của platform
    int spawnDelay;  // Thời gian giữa mỗi lần sinh platform
    int spawnTimer;  // Đếm thời gian để sinh platform tiếp theo

public:
    PlatformManager(std::map<PlatformType, SDL_Texture*> textures) {
        platformTextures = textures;
        scrollSpeed = 5.0f; // Tốc độ mặc định, có thể điều chỉnh
        spawnDelay = 1500; // 1.5 giây giữa mỗi lần sinh platform
        spawnTimer = 0;
    }

    void update(float deltaTime) {
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

    void spawnPlatform() {
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

    void render(Graphics* graphics) {
        // Render tất cả platform active
        for (const auto& platform : platforms) {
            if (platform.active) {
                graphics->renderTexture(platform.texture, platform.rect.x, platform.rect.y);

                // Nếu muốn hiển thị khung va chạm (debug)
                renderDebugCollision(graphics, platform);
            }
        }
    }

    void renderDebugCollision(Graphics* graphics, const Platform& platform) {
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

    const std::vector<Platform>& getPlatforms() const {
        return platforms;
    }

    void setScrollSpeed(float speed) {
        scrollSpeed = speed;
    }

    void clear() {
        platforms.clear();
    }
};

#endif // PLATFORM_H_
*/

// platform.h
#ifndef PLATFORM_H_
#define PLATFORM_H_

#include "common_func.h"
#include "graphics.h"
#include <vector>
#include <map>

enum class PlatformType {
    GRASS_BIG,
    GRASS_MID,
    GRASS_SUPERBIG,
    LAND_MID,
    LAND_SMALL,
    VINE
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
    map<PlatformType, SDL_Texture*> platformTextures;
    vector<Platform> platforms;
    float scrollSpeed; // Tốc độ di chuyển của platform
    int spawnDelay;  // Thời gian giữa mỗi lần sinh platform
    int spawnTimer;  // Đếm thời gian để sinh platform tiếp theo
    // Thêm các phương thức mới
    bool hasLandPlatformAt(int x, int y, int tolerance = 5) const;
    bool hasGrassPlatformAt(int x, int y, int tolerance = 5) const;
    int findValidYForGrass(int x, int kongHeight) const;


public:
    PlatformManager(map<PlatformType, SDL_Texture*> textures);
    void update(float deltaTime);
    void spawnPlatform();
    void render(Graphics* graphics);
    void renderDebugCollision(Graphics* graphics, const Platform& platform);
    const vector<Platform>& getPlatforms() const;
    void setScrollSpeed(float speed);
    void clear();
    // Thêm phương thức kiểm tra vị trí hợp lệ cho chướng ngại vật
    bool isValidPositionForObstacle(int x, int y, int width, int height) const;
};

#endif // PLATFORM_H_
