#include "player.h"
#include "obstacle.h"

Player::Player() {
    x = 150; // Vị trí bắt đầu phù hợp với game
    y = 755;
    velocityX = 0;
    velocityY = 0;
    gravity = 2000.0f; // Tăng trọng lực để rơi nhanh hơn
    jumpForce = -800.0f; // Tăng lực nhảy để nhảy cao hơn
    width = 200; // Kích thước sprite
    height = 167;
    onGround = true; // Bắt đầu trên mặt đất
    state = RUNNING; // Trạng thái mặc định
    collisionRadius = 45; // Bán kính phù hợp với hình ảnh Kong
    showCollision = true; // Mặc định hiển thị khung va chạm
}

Player::~Player() {
    // Dọn dẹp nếu cần
}

void Player::init(SDL_Texture* runTexture, SDL_Texture* slideTex) {
    sprite.texture = runTexture;
    slideTexture = slideTex;
    if (slideTex) {
        slideSprite.texture = slideTex;
    } else {
        slideSprite.texture = runTexture; // Dùng lại sprite chạy nếu không có trượt
    }
    // Khởi tạo clip cho sprite chạy
    for (int i = 0; i < KONGRUN_FRAMES; i++) {
        SDL_Rect clip;
        clip.x = KONGRUN_CLIPS[i][0];
        clip.y = KONGRUN_CLIPS[i][1];
        clip.w = KONGRUN_CLIPS[i][2];
        clip.h = KONGRUN_CLIPS[i][3];
        sprite.clips.push_back(clip);
        slideSprite.clips.push_back(clip); // Giả sử clip trượt giống clip chạy
    }
    sprite.frameDelayMax = 3; // giảm 2 dòng này để nhanh hơn
    slideSprite.frameDelayMax = 3; // Tốc độ animation trượt
}

void Player::update(float deltaTime, const vector<Obstacle>& obstacles) {
    // Áp dụng trọng lực
    velocityY += gravity * deltaTime;

    // Cập nhật vị trí theo chiều dọc
    y += velocityY * deltaTime;

    // Biến để kiểm tra xem nhân vật có đứng trên bất kỳ chướng ngại vật nào không
    bool onAnyGround = false;

    // Kiểm tra va chạm với tất cả các chướng ngại vật
    for (const auto& obstacle : obstacles) {
        if (checkPlatformCollision(obstacle.rect)) {
            onAnyGround = true;
            break;
        }
    }

    // XÓA BỎ phần làm thay đổi vị trí x dựa trên platformSpeed

    // Đảm bảo x luôn giữ nguyên ở vị trí cố định
    x = 150; // Hoặc giá trị ban đầu của x (150 trong constructor)

    // Kiểm tra va chạm với mặt đất cơ bản (nếu không đứng trên chướng ngại vật nào)
    if (!onAnyGround && y > 755) {
        y = 755;
        velocityY = 0;
        onAnyGround = true;
    }

    // Cập nhật trạng thái đứng trên đất
    onGround = onAnyGround;

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
    if (state == SLIDING) {
        slideSprite.tick();
    } else {
        sprite.tick();
    }
}

void Player::render(Graphics* graphics) {
    if (state == SLIDING) {
        graphics->render(x, y, slideSprite);
    } else {
        graphics->render(x, y, sprite);
    }

    // Hiển thị khung va chạm nếu showCollision = true
    if (showCollision) {
        renderDebugCollision(graphics);
    }
}

void Player::renderDebugCollision(Graphics* graphics) {
    // Vẽ hình tròn va chạm thay vì hộp chữ nhật
    renderCircularCollision(graphics);
}

void Player::renderCircularCollision(Graphics* graphics) {
    SDL_Point center = getCollisionCenter();
    int radius = getCollisionRadius();

    // Lưu màu hiện tại
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(graphics->getRenderer(), &r, &g, &b, &a);

    // Thiết lập màu đỏ cho hình tròn va chạm
    SDL_SetRenderDrawColor(graphics->getRenderer(), 255, 0, 0, 255);

    // Vẽ hình tròn bằng thuật toán midpoint circle
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        SDL_RenderDrawPoint(graphics->getRenderer(), center.x + x, center.y + y);
        SDL_RenderDrawPoint(graphics->getRenderer(), center.x + y, center.y + x);
        SDL_RenderDrawPoint(graphics->getRenderer(), center.x - y, center.y + x);
        SDL_RenderDrawPoint(graphics->getRenderer(), center.x - x, center.y + y);
        SDL_RenderDrawPoint(graphics->getRenderer(), center.x - x, center.y - y);
        SDL_RenderDrawPoint(graphics->getRenderer(), center.x - y, center.y - x);
        SDL_RenderDrawPoint(graphics->getRenderer(), center.x + y, center.y - x);
        SDL_RenderDrawPoint(graphics->getRenderer(), center.x + x, center.y - y);

        if (err <= 0) {
            y += 1;
            err += 2*y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2*x + 1;
        }
    }

    // Thêm vẽ điểm tâm (màu xanh lá)
    SDL_SetRenderDrawColor(graphics->getRenderer(), 0, 255, 0, 255);
    // Vẽ điểm tâm lớn hơn để dễ nhìn
    for (int i = -2; i <= 2; i++) {
        for (int j = -2; j <= 2; j++) {
            SDL_RenderDrawPoint(graphics->getRenderer(), center.x + i, center.y + j);
        }
    }

    // Vẽ hai đường chéo qua tâm để dễ xác định vị trí
    SDL_SetRenderDrawColor(graphics->getRenderer(), 0, 255, 0, 128);
    SDL_RenderDrawLine(graphics->getRenderer(), center.x - radius, center.y - radius, center.x + radius, center.y + radius);
    SDL_RenderDrawLine(graphics->getRenderer(), center.x - radius, center.y + radius, center.x + radius, center.y - radius);

    // Khôi phục màu ban đầu
    SDL_SetRenderDrawColor(graphics->getRenderer(), r, g, b, a);
}

SDL_Point Player::getCollisionCenter() const {
    SDL_Point center;
    // Tâm của hình tròn va chạm, có thể điều chỉnh để phù hợp với hình dáng nhân vật
    center.x = static_cast<int>(x + 57); // tâm cách mép trái khoảng x=45
    center.y = static_cast<int>(y + 82); // tâm cách mép trên của ảnh khoảng y=82
    return center;
}

int Player::getCollisionRadius() const {
    // Bạn có thể điều chỉnh bán kính theo trạng thái nhân vật nếu cần
    if (state == SLIDING) {
        return static_cast<int>(collisionRadius * 0.8); // Giảm bán kính khi trượt
    }
    return collisionRadius;
}

bool Player::checkCircularCollision(const SDL_Point& otherCenter, int otherRadius) const {
    SDL_Point myCenter = getCollisionCenter();
    int myRadius = getCollisionRadius();

    // Tính khoảng cách bình phương giữa hai tâm
    int deltaX = myCenter.x - otherCenter.x;
    int deltaY = myCenter.y - otherCenter.y;
    int distanceSquared = (deltaX * deltaX) + (deltaY * deltaY);

    // Kiểm tra va chạm (tổng bán kính lớn hơn hoặc bằng khoảng cách)
    int radiusSum = myRadius + otherRadius;
    return distanceSquared <= (radiusSum * radiusSum);
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
        SDL_Log("start to slide");
    }
}

void Player::stopSliding() {
    if (state == SLIDING) {
        state = RUNNING;
        // Đặt lại hộp va chạm nếu bạn đã sửa đổi nó
        SDL_Log("Stop to slide");
    }
}

SDL_Rect Player::getCollisionBox() const {
    SDL_Rect box;

    // Tạo hộp va chạm nhỏ hơn so với sprite thực tế
    int paddingX = static_cast<int>(width * 0.35);  // Ép kiểu rõ ràng
    int paddingY = static_cast<int>(height * 0.2);  // Ép kiểu rõ ràng

    box.x = static_cast<int>(x) + paddingX;
    box.y = static_cast<int>(y) + paddingY;
    box.w = static_cast<int>(width) - (paddingX * 2);
    box.h = static_cast<int>(height) - (paddingY * 2);

    // Điều chỉnh hộp va chạm khi trượt
    if (state == SLIDING) {
        box.h = static_cast<int>(height * 0.4);  // Ép kiểu rõ ràng
        box.y = static_cast<int>(y) + static_cast<int>(height * 0.5);  // Ép kiểu rõ ràng
    }

    return box;
}

bool Player::checkPlatformCollision(const SDL_Rect& obstacle) {
    // Tính toán trung tâm của người chơi
    SDL_Point playerCenter = getCollisionCenter();
    int playerRadius = getCollisionRadius();

    // Chỉ kiểm tra va chạm khi đang rơi xuống
    if (velocityY > 0) {
        // Tính toán điểm gần nhất từ hình tròn đến hình chữ nhật
        int closestX = std::max(obstacle.x, std::min(playerCenter.x, obstacle.x + obstacle.w));
        int closestY = std::max(obstacle.y, std::min(playerCenter.y, obstacle.y + obstacle.h));

        // Tính khoảng cách từ điểm gần nhất đến tâm hình tròn
        int deltaX = playerCenter.x - closestX;
        int deltaY = playerCenter.y - closestY;
        int distanceSquared = (deltaX * deltaX) + (deltaY * deltaY);

        // Kiểm tra xem khoảng cách có nhỏ hơn bán kính không
        if (distanceSquared <= (playerRadius * playerRadius)) {
            // Chỉ xử lý va chạm với phần trên của nền
            if (closestY <= obstacle.y + 15) {
                // Đặt người chơi lên trên chướng ngại vật
                y = obstacle.y - height + (playerCenter.y - y - playerRadius);
                velocityY = 0;
                setOnGround(true);
                return true;
            }
        }
    }

    return false;
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

void Player::setCollisionRadius(int radius) {
    collisionRadius = radius;
}

void Player::toggleCollisionDisplay() {
    showCollision = !showCollision;
}
