#include "menu.h"
#include "gameloop.h"
#include <SDL_ttf.h>

Menu::Menu(Graphics& graphics, GameLoop& gameLoop) : gameLoop(gameLoop), isOffButtonActive(true), isCountingDown(false), countdownValue(0) {
    homeplayTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/MENU/homeplay.png");
    if (!homeplayTexture) {
        // SDL_Log("Failed to load homeplay texture: %s", SDL_GetError());
    }
    menuTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/MENU/menu.png");
    if (!menuTexture) {
        // SDL_Log("Failed to load menu texture: %s", SDL_GetError());
    }
    offTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/MENU/off.png"); // Tải nút off
    if (!offTexture) {
        // SDL_Log("Failed to load off texture: %s", SDL_GetError());
    }
    onTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/MENU/on.png"); // Tải nút on
    if (!onTexture) {
        // SDL_Log("Failed to load on texture: %s", SDL_GetError());
    }
    menuVisible = false;
}

Menu::~Menu() {
    SDL_DestroyTexture(homeplayTexture);
    SDL_DestroyTexture(menuTexture);
    SDL_DestroyTexture(offTexture);
    SDL_DestroyTexture(onTexture);
}

void Menu::togglePause(GameState& gameState, bool& isPaused) {
    if (isOffButtonActive) {
        // Chuyển sang pause, hiển thị menu, đổi nút off thành on
        isPaused = true;
        menuVisible = true;
        isOffButtonActive = false;
        // SDL_Log("Paused: isPaused set to true");
    } else {
        // Ẩn menu, bắt đầu đếm ngược, sau đó tiếp tục trò chơi
        menuVisible = false;
        startCountdown();
        // KHÔNG thay đổi isPaused ở đây, để nó vẫn là true cho đến khi đếm ngược xong
        // SDL_Log("Starting countdown, isPaused still true: %d", isPaused);
    }
}

void Menu::startCountdown() {
    isCountingDown = true;
    countdownValue = 3;
    countdownStartTime = SDL_GetTicks();
    // SDL_Log("Countdown started, countdownValue: %d", countdownValue);
}

SDL_Texture* Menu::createCountdownTexture(Graphics& graphics, int value) {
    TTF_Font* font = TTF_OpenFont("D:/projectBTL/bananakong/font/Drawing_Kids.ttf", 100); // Sử dụng font có sẵn
    if (!font) {
        // SDL_Log("Failed to load font for countdown: %s", TTF_GetError());
        return nullptr;
    }

    std::string text = std::to_string(value);
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), white);
    if (!surface) {
        // SDL_Log("Failed to create countdown surface: %s", TTF_GetError());
        TTF_CloseFont(font);
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(graphics.getRenderer(), surface);
    SDL_FreeSurface(surface);
    TTF_CloseFont(font);
    return texture;
}

void Menu::handleEvents(SDL_Event& e, GameState& gameState, bool& isPaused) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        if (gameState == HOMEPLAY) {
            // Kiểm tra vùng nhấn: x từ 0-->1346, y từ 0-->963 để tap to start
            if (mouseX >= 0 && mouseX <= 1346 && mouseY >= 0 && mouseY <= 963) {
                gameLoop.reset();
                gameState = PLAYING;
            }
            // Kiểm tra vùng nhấn: x từ 1346-->1500, y từ 0-->963 để mở menu
            else if (mouseX >= 1346 && mouseX <= 1500 && mouseY >= 0 && mouseY <= 963) {
                menuVisible = true;
                gameState = MENU;
            }
        }
        else if (gameState == PLAYING && !isCountingDown) {
            // Chỉ xử lý click nút off/on khi không đang đếm ngược
            // Kiểm tra vùng nhấn vào nút off/on (giả định kích thước 53x53)
            if (mouseX >= 1447 && mouseX <= 1500 && mouseY >= 0 && mouseY <= 53) {
                togglePause(gameState, isPaused);
            }
        }
        else if (gameState == MENU && menuVisible) {
            // Nhấn ngoài nút on để quay lại HOMEPLAY
            if (mouseX < 1447 || mouseX > 1500 || mouseY < 0 || mouseY > 53) {
                menuVisible = false;
                gameState = HOMEPLAY;
            }
            // Kiểm tra nhấn nút on để tiếp tục (bắt đầu đếm ngược)
            else if (gameState == PLAYING && mouseX >= 1447 && mouseX <= 1500 && mouseY >= 0 && mouseY <= 53 && !isOffButtonActive) {
                togglePause(gameState, isPaused);
            }
        }
        else if (gameState == GAME_OVER) {
            // Nhấn bất kỳ đâu để restart game từ GAME_OVER
            gameLoop.reset();
            gameState = PLAYING;
            // Đảm bảo game không ở trạng thái pause khi restart
            isPaused = false;
            isOffButtonActive = true;
            isCountingDown = false;
        }
    }
}

void Menu::render(Graphics& graphics, GameState gameState, bool& isPaused) {
    if (gameState == HOMEPLAY) {
        graphics.renderTexture(homeplayTexture, 0, 0);
        if (menuVisible) {
            graphics.renderTexture(menuTexture, 0, 170); // Chỉ hiển thị menu, không có nút ON
        }
    }
    else if (gameState == PLAYING) {
        if (!isPaused && !isCountingDown) {
            graphics.renderTexture(offTexture, 1447, 0); // Vẽ nút off khi không pause và không đếm ngược
        } else {
            if (menuVisible) {
                graphics.renderTexture(menuTexture, 0, 170); // Vẽ menu khi pause
                graphics.renderTexture(onTexture, 1447, 0); // Vẽ nút on khi pause, chỉ trong PLAYING
            } else if (isCountingDown) {
                // Vẽ đếm ngược và giữ nút on trong lúc đếm
                graphics.renderTexture(onTexture, 1447, 0); // Vẫn giữ nút on trong đếm ngược

                // Vẽ đếm ngược
                Uint32 currentTime = SDL_GetTicks();
                Uint32 elapsedTime = currentTime - countdownStartTime;

                // Kiểm tra thời gian từ lần cuối cập nhật countdown
                if (elapsedTime >= 1000) {
                    countdownValue--;
                    countdownStartTime = currentTime; // Cập nhật thời điểm bắt đầu cho giây tiếp theo

                    if (countdownValue <= 0) {
                        isCountingDown = false;
                        isPaused = false; // Tiếp tục trò chơi sau khi đếm xong
                        isOffButtonActive = true; // Chuyển thành off sau khi đếm xong
                        return; // Thoát ngay để tránh render số 0
                    }
                }

                // Vẽ số đếm ngược hiện tại
                SDL_Texture* countdownTexture = createCountdownTexture(graphics, countdownValue);
                if (countdownTexture) {
                    int w, h;
                    SDL_QueryTexture(countdownTexture, nullptr, nullptr, &w, &h);
                    SDL_Rect dstRect = {800 - w / 2, 400 - h / 2, w, h}; // Đặt giữa màn hình
                    graphics.renderTexture(countdownTexture, dstRect.x, dstRect.y);
                    SDL_DestroyTexture(countdownTexture);
                }
            } else {
                graphics.renderTexture(offTexture, 1447, 0); // Vẽ nút off sau khi đếm xong
            }
        }
    }
    else if (gameState == MENU && menuVisible) {
        graphics.renderTexture(menuTexture, 0, 170);
        if (gameState == PLAYING) {
            graphics.renderTexture(onTexture, 1447, 0); // Vẽ nút on khi ở PLAYING
        }
    }
}
