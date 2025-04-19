#ifndef PLAYER_H_
#define PLAYER_H_
#include "common_func.h"
#include "graphics.h"
#include <vector>
#include <map>

// Khai báo tiền định nghĩa
struct Obstacle;
enum class ObstacleType;

enum PlayerState {
    IDLE,
    RUNNING,
    JUMPING,
    FALLING,
    SLIDING
};

class Player {
private:
    float x, y;
    float velocityX, velocityY;
    PlayerState state;
    float gravity;
    float jumpForce;
    int width, height;
    bool onGround;
    Sprite sprite; // Sprite chạy hiện có
    Sprite slideSprite; // Sprite mới cho trượt
    SDL_Texture* slideTexture; // Texture cho trượt

    // Thêm phương thức kiểm tra va chạm với nền
    bool checkPlatformCollision(const SDL_Rect& obstacle);

    // Thêm thuộc tính cho hình tròn va chạm
    int collisionRadius; // Bán kính hình tròn va chạm

    // Biến cờ để bật/tắt hiển thị khung va chạm
    bool showCollision;

public:
    Player();
    ~Player();

    void init(SDL_Texture* runTexture, SDL_Texture* slideTex = nullptr);
    void update(float deltaTime, const std::vector<SDL_Rect>& platforms);
    void render(Graphics* graphics);
    void renderDebugCollision(Graphics* graphics);
    void renderCircularCollision(Graphics* graphics); // Vẽ hình tròn va chạm

    void jump();
    void slide();
    void stopSliding();

    // Phương thức cho va chạm hình chữ nhật (giữ lại cho khả năng tương thích)
    SDL_Rect getCollisionBox() const;

    // Phương thức mới cho va chạm hình tròn
    SDL_Point getCollisionCenter() const; // Lấy tâm hình tròn va chạm
    int getCollisionRadius() const; // Lấy bán kính hình tròn va chạm
    bool checkCircularCollision(const SDL_Point& otherCenter, int otherRadius) const; // Kiểm tra va chạm hình tròn

    void setCollisionRadius(int radius); // Thiết lập bán kính va chạm
    void setOnGround(bool grounded);
    bool isOnGround() const { return onGround; }

    float getX() const { return x; }
    float getY() const { return y; }
    void setPosition(float newX, float newY);

    PlayerState getState() const { return state; }

    // Phương thức để bật/tắt hiển thị khung va chạm
    void toggleCollisionDisplay();
    bool isCollisionDisplayed() const { return showCollision; }
};

#endif // PLAYER_H_
