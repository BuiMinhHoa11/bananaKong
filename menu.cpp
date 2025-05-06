#include "menu.h"
#include "gameloop.h"
#include <SDL_ttf.h>

Menu::Menu(Graphics& graphics, GameLoop& gameLoop, AudioManager& audioManager)
    : gameLoop(gameLoop), audioManager(audioManager), menuVisible(false),
      isOffButtonActive(true), isCountingDown(false), countdownValue(0),
      isReviveCountingDown(false), reviveCountdownValue(5.0f) {
    homeplayTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/MENU/homeplay.png");
    if (!homeplayTexture) {
        SDL_Log("Failed to load homeplay texture: %s", SDL_GetError());
    }
    menuTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/MENU/menu.png");
    if (!menuTexture) {
        SDL_Log("Failed to load menu texture: %s", SDL_GetError());
    }
    offTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/MENU/off.png");
    if (!offTexture) {
        SDL_Log("Failed to load off texture: %s", SDL_GetError());
    }
    onTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/MENU/on.png");
    if (!onTexture) {
        SDL_Log("Failed to load on texture: %s", SDL_GetError());
    }
    optionsTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/MENU/options.png");
    if (!optionsTexture) {
        SDL_Log("Failed to load options texture: %s", SDL_GetError());
    }
    backTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/MENU/back.png");
    if (!backTexture) {
        SDL_Log("Failed to load back texture: %s", SDL_GetError());
    }
    reviveTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/MENU/revive.png");
    if (!reviveTexture) {
        SDL_Log("Failed to load revive texture: %s", SDL_GetError());
    }
}

Menu::~Menu() {
    SDL_DestroyTexture(homeplayTexture);
    SDL_DestroyTexture(menuTexture);
    SDL_DestroyTexture(offTexture);
    SDL_DestroyTexture(onTexture);
    SDL_DestroyTexture(optionsTexture);
    SDL_DestroyTexture(backTexture);
    SDL_DestroyTexture(reviveTexture);
}

void Menu::togglePause(GameState& gameState, MenuState& menuState, bool& isPaused) {
    if (isOffButtonActive) {
        // Từ OFF (đang chơi) sang ON (tạm dừng)
        isPaused = true;
        menuVisible = true;
        isOffButtonActive = false;
        stateHistory.push_back(gameState);
        menuState = MENU;
        audioManager.playSound(SoundType::CLICK);
    } else {
        // Từ ON (tạm dừng) sang OFF (đang chơi)
        menuVisible = false;
        menuState = NONE;
        startCountdown();
        audioManager.playSound(SoundType::CLICK);
    }
}

void Menu::startCountdown() {
    isCountingDown = true;
    countdownValue = 3;
    countdownStartTime = SDL_GetTicks();
    audioManager.playSound(SoundType::COUNT_DOWN);
}

void Menu::startReviveCountdown() {
    isReviveCountingDown = true;
    reviveCountdownValue = 5.0f; // 5 giây để quyết định hồi sinh
    reviveCountdownStartTime = SDL_GetTicks();
    audioManager.playSound(SoundType::COUNT_DOWN);
}

SDL_Texture* Menu::createCountdownTexture(Graphics& graphics, int value) {
    TTF_Font* font = TTF_OpenFont("D:/projectBTL/bananakong/font/Gameplay.ttf", 200); // Font nhỏ cho số đếm ngược
    if (!font) {
        SDL_Log("Failed to load font for countdown: %s", TTF_GetError());
        return nullptr;
    }

    std::string text = std::to_string(value);
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), white);
    if (!surface) {
        SDL_Log("Failed to create countdown surface: %s", TTF_GetError());
        TTF_CloseFont(font);
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(graphics.getRenderer(), surface);
    SDL_FreeSurface(surface);
    TTF_CloseFont(font);
    return texture;
}

void Menu::updateReviveCountdown(GameState& gameState, MenuState& menuState, bool& isPaused) {
    if (menuState == REVIVE && gameState == GAME_OVER && isReviveCountingDown) {
        Uint32 currentTime = SDL_GetTicks();
        float elapsedTime = (currentTime - reviveCountdownStartTime) / 1000.0f;
        int remainingTime = static_cast<int>(ceil(reviveCountdownValue - elapsedTime));
        if (remainingTime <= 0) {
            // Hết thời gian, tự động chuyển về HOMEPLAY
            isReviveCountingDown = false;
            gameState = HOMEPLAY;
            menuState = NONE;
            menuVisible = false;
            isPaused = false;
            isOffButtonActive = true;
            gameLoop.reset();
            audioManager.playMusic(MusicType::HOMEPLAY);
        }
    }
}

void Menu::handleEvents(SDL_Event& e, GameState& gameState, MenuState& menuState, bool& isPaused) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        if (gameState == HOMEPLAY && menuState == NONE) {
            if (mouseX >= 0 && mouseX <= 1346 && mouseY >= 0 && mouseY <= 963) {
                gameLoop.reset();
                gameState = PLAYING;
                isPaused = false;
                isOffButtonActive = true;
                isCountingDown = false;
                audioManager.playSound(SoundType::CLICK);
            } else if (mouseX >= 1346 && mouseX <= 1500 && mouseY >= 0 && mouseY <= 963) {
                stateHistory.push_back(gameState);
                menuVisible = true;
                menuState = MENU;
                audioManager.playSound(SoundType::SLIDE); // Âm thanh slide khi menu xổ ra
            }
        } else if (gameState == HOMEPLAY && menuState == MENU && menuVisible) {
            // Nhấn bất kỳ đâu để thu menu trong HOMEPLAY
            if (mouseX >= 780 && mouseX <= 1197 && mouseY >= 408 && mouseY <= 545) {
                stateHistory.push_back(menuState);
                menuState = OPTIONS;
                audioManager.playSound(SoundType::CLICK);
            } else {
                menuVisible = false;
                menuState = NONE;
                if (!stateHistory.empty()) {
                    stateHistory.pop_back(); // Xóa trạng thái HOMEPLAY khỏi lịch sử
                }
                audioManager.playSound(SoundType::SLIDE); // Âm thanh slide khi menu xổ vào
            }
        } else if (gameState == PLAYING && menuState == NONE && !isCountingDown) {
            if (mouseX >= 1447 && mouseX <= 1500 && mouseY >= 0 && mouseY <= 53) {
                togglePause(gameState, menuState, isPaused);
            }
        } else if (menuState == MENU && menuVisible && gameState != HOMEPLAY) {
            if (mouseX >= 780 && mouseX <= 1197 && mouseY >= 408 && mouseY <= 545) {
                stateHistory.push_back(menuState);
                menuState = OPTIONS;
                audioManager.playSound(SoundType::CLICK);
            } else if (mouseX >= 1447 && mouseX <= 1500 && mouseY >= 0 && mouseY <= 53 && !isOffButtonActive) {
                togglePause(gameState, menuState, isPaused);
            }
        } else if (menuState == OPTIONS) {
            int backW, backH;
            SDL_QueryTexture(backTexture, nullptr, nullptr, &backW, &backH);
            if (mouseX >= 50 && mouseX <= 50 + backW && mouseY >= 50 && mouseY <= 50 + backH) {
                if (!stateHistory.empty()) {
                    auto prevState = stateHistory.back();
                    stateHistory.pop_back();
                    if (auto* prevMenuState = std::get_if<MenuState>(&prevState)) {
                        menuState = *prevMenuState;
                        menuVisible = true;
                    } else if (auto* prevGameState = std::get_if<GameState>(&prevState)) {
                        gameState = *prevGameState;
                        menuState = NONE;
                        menuVisible = false;
                    }
                } else {
                    gameState = HOMEPLAY;
                    menuState = NONE;
                    menuVisible = false;
                }
                audioManager.playSound(SoundType::CLICK);
            }
        } else if (menuState == REVIVE && gameState == GAME_OVER && isReviveCountingDown) {
            // Nhấn vào khu vực cố định để hồi sinh chỉ khi còn trong thời gian
            int reviveW, reviveH;
            SDL_QueryTexture(reviveTexture, nullptr, nullptr, &reviveW, &reviveH);
            if (mouseX >= 570 && mouseX <= 950 && mouseY >= 445 && mouseY <= 625) {
                gameLoop.revivePlayer(); // Hồi sinh Kong, giữ nguyên chuối và khoảng cách
                gameState = PLAYING;
                menuState = NONE;
                isPaused = false;
                isOffButtonActive = true;
                isReviveCountingDown = false;
                audioManager.playSound(SoundType::CLICK);
            }
        }
    }
}

void Menu::render(Graphics& graphics, GameState gameState, MenuState menuState, bool& isPaused) {
    if (gameState == HOMEPLAY) {
        graphics.renderTexture(homeplayTexture, 0, 0);
        if (menuState == MENU && menuVisible) {
            graphics.renderTexture(menuTexture, 0, 170);
        } else if (menuState == OPTIONS) {
            graphics.renderTexture(optionsTexture, 300, 210);
            graphics.renderTexture(backTexture, 50, 50);
        }
    } else if (gameState == PLAYING || gameState == GAME_OVER) {
        if (menuState == MENU && menuVisible) {
            graphics.renderTexture(menuTexture, 0, 170);
            graphics.renderTexture(onTexture, 1447, 0);
        } else if (menuState == OPTIONS) {
            graphics.renderTexture(optionsTexture, 300, 210);
            graphics.renderTexture(backTexture, 50, 50);
        } else if (menuState == REVIVE && gameState == GAME_OVER && isReviveCountingDown) {
            int reviveW, reviveH;
            SDL_QueryTexture(reviveTexture, nullptr, nullptr, &reviveW, &reviveH);
            int reviveX = (1500 - reviveW) / 2; // Trung tâm màn hình
            int reviveY = (963 - reviveH) / 2;
            graphics.renderTexture(reviveTexture, reviveX, reviveY);

            Uint32 currentTime = SDL_GetTicks();
            float elapsedTime = (currentTime - reviveCountdownStartTime) / 1000.0f;
            int remainingTime = static_cast<int>(ceil(reviveCountdownValue - elapsedTime));
            // Hiển thị số đếm ngược ở mép trái bảng revive
            SDL_Texture* countdownTexture = createCountdownTexture(graphics, remainingTime);
            if (countdownTexture) {
                int w, h;
                SDL_QueryTexture(countdownTexture, nullptr, nullptr, &w, &h);
                graphics.renderTexture(countdownTexture, reviveX + 36, reviveY + 19);
                SDL_DestroyTexture(countdownTexture);
            }
        } else if (isCountingDown) {
            graphics.renderTexture(onTexture, 1447, 0);
            Uint32 currentTime = SDL_GetTicks();
            Uint32 elapsedTime = currentTime - countdownStartTime;
            if (elapsedTime >= 1000) {
                countdownValue--;
                countdownStartTime = currentTime;
                if (countdownValue <= 0) {
                    isCountingDown = false;
                    isPaused = false;
                    isOffButtonActive = true;
                    menuState = NONE;
                    graphics.renderTexture(offTexture, 1447, 0);
                    return;
                }
                audioManager.playSound(SoundType::COUNT_DOWN);
            }
            SDL_Texture* countdownTexture = createCountdownTexture(graphics, countdownValue);
            if (countdownTexture) {
                int w, h;
                SDL_QueryTexture(countdownTexture, nullptr, nullptr, &w, &h);
                SDL_Rect dstRect = {800 - w / 2, 400 - h / 2, w, h};
                graphics.renderTexture(countdownTexture, dstRect.x, dstRect.y);
                SDL_DestroyTexture(countdownTexture);
            }
        } else if ((gameState == PLAYING || gameState == GAME_OVER) && menuState == NONE) {
            graphics.renderTexture(offTexture, 1447, 0);
        }
    }
}
