#include "player.h"

Player::Player() {
    x = 120; // Vị trí bắt đầu phù hợp với game
    y = 755;
    velocityX = 0;
    velocityY = 0;
    gravity = 2000.0f; // Tăng trọng lực để rơi nhanh hơn
    jumpForce = -800.0f; // Tăng lực nhảy để nhảy cao hơn
    width = 200; // Kích thước sprite
    height = 167;
    onGround = true; // Bắt đầu trên mặt đất
    state = RUNNING; // Trạng thái mặc định
}

Player::~Player() {
    // Dọn dẹp nếu cần
}

void Player::init(SDL_Texture* texture) {
    sprite.texture = texture;
    // Khởi tạo sprite với KONGRUN_CLIPS và KONGRUN_FRAMES
    for (int i = 0; i < KONGRUN_FRAMES; i++) {
        SDL_Rect clip;
        clip.x = KONGRUN_CLIPS[i][0];
        clip.y = KONGRUN_CLIPS[i][1];
        clip.w = KONGRUN_CLIPS[i][2];
        clip.h = KONGRUN_CLIPS[i][3];
        sprite.clips.push_back(clip);
    }
    sprite.frameDelayMax = 5; // Tốc độ animation
}

void Player::update(float deltaTime) {
    // Áp dụng trọng lực
    velocityY += gravity * deltaTime;

    // Cập nhật vị trí
    y += velocityY * deltaTime;

    // Kiểm tra va chạm với mặt đất
    if (y > 755) {
        y = 755;
        velocityY = 0;
        onGround = true;
    } else {
        onGround = false;
    }

    // Xác định trạng thái người chơi
    if (!onGround) {
        if (velocityY < 0) {
            state = JUMPING;
        } else {
            state = FALLING;
        }
    } else {
        if (state == SLIDING) {
            // Giữ trạng thái trượt nếu đang trượt
        } else {
            state = RUNNING; // Mặc định là chạy khi trên mặt đất
        }
    }

    // Cập nhật animation
    sprite.tick();
}

void Player::render(Graphics* graphics) {
    graphics->render(x, y, sprite);
}

void Player::jump() {
    if (onGround) {
        velocityY = jumpForce;
        onGround = false;
        state = JUMPING;
    }
}

void Player::slide() {
    if (onGround) {
        state = SLIDING;
        // Có thể điều chỉnh hộp va chạm ở đây
    }
}

void Player::stopSliding() {
    if (state == SLIDING) {
        state = RUNNING;
        // Đặt lại hộp va chạm nếu bạn đã sửa đổi nó
    }
}

SDL_Rect Player::getCollisionBox() const {
    SDL_Rect box;
    box.x = static_cast<int>(x);
    box.y = static_cast<int>(y);
    box.w = width;
    box.h = height;

    // Điều chỉnh hộp va chạm khi trượt
    if (state == SLIDING) {
        box.h = height / 2; // Chiều cao thấp hơn khi trượt
        box.y += height / 2;
    }

    return box;
}

void Player::setOnGround(bool grounded) {
    onGround = grounded;
    if (grounded && (state == JUMPING || state == FALLING)) {
        state = RUNNING;
    }
}

void Player::setPosition(float newX, float newY) {
    x = newX;
    y = newY;
}
