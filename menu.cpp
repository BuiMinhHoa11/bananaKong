#include "menu.h"
#include "gameloop.h"
#include <cmath>

Menu::Menu(Graphics& graphics, GameLoop& gameLoop, AudioManager& audioManager)
    : gameLoop(gameLoop), audioManager(audioManager) {
    homeplayTexture = nullptr;
    menuTexture = nullptr;
    offTexture = nullptr;
    onTexture = nullptr;
    optionsTexture = nullptr;
    backTexture = nullptr;
    reviveTexture = nullptr;
    musicOnTexture = nullptr;
    musicOffTexture = nullptr;
    effectOnTexture = nullptr;
    effectOffTexture = nullptr;
    recordTexture = nullptr;
    countdownFont = nullptr;
    reviveCountdownFont = nullptr;
    recordFont = nullptr;
    menuVisible = false;
    isOffButtonActive = true;
    isCountingDown = false;
    countdownValue = 0;
    countdownStartTime = 0;
    isReviveCountingDown = false;
    reviveCountdownValue = 5.0f;
    reviveCountdownStartTime = 0;

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
    musicOnTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/MENU/music_on.png");
    if (!musicOnTexture) {
        SDL_Log("Failed to load music_on texture: %s", SDL_GetError());
    }
    musicOffTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/MENU/music_off.png");
    if (!musicOffTexture) {
        SDL_Log("Failed to load music_off texture: %s", SDL_GetError());
    }
    effectOnTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/MENU/effect_on.png");
    if (!effectOnTexture) {
        SDL_Log("Failed to load effect_on texture: %s", SDL_GetError());
    }
    effectOffTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/MENU/effect_off.png");
    if (!effectOffTexture) {
        SDL_Log("Failed to load effect_off texture: %s", SDL_GetError());
    }
    recordTexture = graphics.loadTexture("D:/projectBTL/bananakong/image/MENU/record.png");
    if (!recordTexture) {
        SDL_Log("Failed to load record texture: %s", SDL_GetError());
    }

    // Khởi tạo các font
    countdownFont = TTF_OpenFont("D:/projectBTL/bananakong/font/Gameplay.ttf", 200);
    if (!countdownFont) {
        SDL_Log("Failed to load countdown font: %s", TTF_GetError());
    }
    reviveCountdownFont = TTF_OpenFont("D:/projectBTL/bananakong/font/Gameplay.ttf", 40);
    if (!reviveCountdownFont) {
        SDL_Log("Failed to load revive countdown font: %s", TTF_GetError());
    }
    recordFont = TTF_OpenFont("D:/projectBTL/bananakong/font/Gameplay.ttf", 40);
    if (!recordFont) {
        SDL_Log("Failed to load record font: %s", TTF_GetError());
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
    SDL_DestroyTexture(musicOnTexture);
    SDL_DestroyTexture(musicOffTexture);
    SDL_DestroyTexture(effectOnTexture);
    SDL_DestroyTexture(effectOffTexture);
    SDL_DestroyTexture(recordTexture);

    if (countdownFont) {
        TTF_CloseFont(countdownFont);
        countdownFont = nullptr;
    }
    if (reviveCountdownFont) {
        TTF_CloseFont(reviveCountdownFont);
        reviveCountdownFont = nullptr;
    }
    if (recordFont) {
        TTF_CloseFont(recordFont);
        recordFont = nullptr;
    }
}

void Menu::togglePause(GameState& gameState, MenuState& menuState, bool& isPaused) {
    if (isOffButtonActive) {
        isPaused = true;
        menuVisible = true;
        isOffButtonActive = false;
        stateHistory.push(gameState);
        menuState = MenuState::MENU;
        audioManager.playSound(SoundType::CLICK);
    } else {
        menuVisible = false;
        menuState = MenuState::NONE;
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
    reviveCountdownValue = 5.0f;
    reviveCountdownStartTime = SDL_GetTicks();
    audioManager.playSound(SoundType::COUNT_DOWN);
}

void Menu::updateReviveCountdown(GameState& gameState, MenuState& menuState, bool& isPaused) {
    if (menuState == MenuState::REVIVE && gameState == GameState::GAME_OVER && isReviveCountingDown) {
        Uint32 currentTime = SDL_GetTicks();
        float elapsedTime = (currentTime - reviveCountdownStartTime) / 1000.0f;
        int remainingTime = static_cast<int>(ceil(reviveCountdownValue - elapsedTime));
        if (remainingTime <= 0) {
            isReviveCountingDown = false;
            gameState = GameState::HOMEPLAY;
            menuState = MenuState::NONE;
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
        if (gameState == GameState::HOMEPLAY && menuState == MenuState::NONE) {
            if (mouseX >= 0 && mouseX <= 1346 && mouseY >= 0 && mouseY <= 963) {
                gameLoop.reset();
                gameState = GameState::PLAYING;
                isPaused = false;
                isOffButtonActive = true;
                isCountingDown = false;
                audioManager.playSound(SoundType::CLICK);
            } else if (mouseX >= 1346 && mouseX <= 1500 && mouseY >= 0 && mouseY <= 963) {
                stateHistory.push(gameState);
                menuVisible = true;
                menuState = MenuState::MENU;
                audioManager.playSound(SoundType::SLIDE);
            }
        } else if (gameState == GameState::HOMEPLAY && menuState == MenuState::MENU && menuVisible) {
            if (mouseX >= 780 && mouseX <= 1197 && mouseY >= 408 && mouseY <= 545) {
                stateHistory.push(menuState);
                menuState = MenuState::OPTIONS;
                audioManager.playSound(SoundType::CLICK);
            } else if (mouseX >= 780 && mouseX <= 1200 && mouseY >= 580 && mouseY <= 720) {
                stateHistory.push(menuState);
                menuState = MenuState::NEW_PANEL;
                audioManager.playSound(SoundType::CLICK);
            } else if (mouseX >= 230 && mouseX <= 615 && mouseY >= 545 && mouseY <= 690) {
                gameLoop.reset();
                gameState = GameState::PLAYING;
                menuState = MenuState::NONE;
                menuVisible = false;
                isPaused = false;
                isOffButtonActive = true;
                audioManager.playSound(SoundType::CLICK);
                audioManager.playMusic(MusicType::LOOP);
                while (!stateHistory.empty()) {
                    stateHistory.pop();
                }
            } else {
                menuVisible = false;
                menuState = MenuState::NONE;
                if (!stateHistory.empty()) {
                    stateHistory.pop();
                }
                audioManager.playSound(SoundType::SLIDE);
            }
        } else if (gameState == GameState::PLAYING && menuState == MenuState::NONE && !isCountingDown) {
            if (mouseX >= 1447 && mouseX <= 1500 && mouseY >= 0 && mouseY <= 53) {
                togglePause(gameState, menuState, isPaused);
            }
        } else if (menuState == MenuState::MENU && menuVisible && gameState != GameState::HOMEPLAY) {
            if (mouseX >= 780 && mouseX <= 1197 && mouseY >= 408 && mouseY <= 545) {
                stateHistory.push(menuState);
                menuState = MenuState::OPTIONS;
                audioManager.playSound(SoundType::CLICK);
            } else if (mouseX >= 780 && mouseX <= 1200 && mouseY >= 580 && mouseY <= 720) {
                stateHistory.push(menuState);
                menuState = MenuState::NEW_PANEL;
                audioManager.playSound(SoundType::CLICK);
            } else if (mouseX >= 1447 && mouseX <= 1500 && mouseY >= 0 && mouseY <= 53 && !isOffButtonActive) {
                togglePause(gameState, menuState, isPaused);
            }
        } else if (menuState == MenuState::OPTIONS) {
            int backW, backH;
            SDL_QueryTexture(backTexture, nullptr, nullptr, &backW, &backH);
            if (mouseX >= 50 && mouseX <= 50 + backW && mouseY >= 50 && mouseY <= 50 + backH) {
                if (!stateHistory.empty()) {
                    auto prevState = stateHistory.top();
                    stateHistory.pop();
                    if (auto* prevMenuState = std::get_if<MenuState>(&prevState)) {
                        menuState = *prevMenuState;
                        menuVisible = true;
                    } else if (auto* prevGameState = std::get_if<GameState>(&prevState)) {
                        gameState = *prevGameState;
                        menuState = MenuState::NONE;
                        menuVisible = false;
                    }
                } else {
                    gameState = GameState::HOMEPLAY;
                    menuState = MenuState::NONE;
                    menuVisible = false;
                }
                audioManager.playSound(SoundType::CLICK);
            } else if (mouseX >= 458 && mouseX <= 700 && mouseY >= 340 && mouseY <= 555) {
                audioManager.setMusicEnabled(!audioManager.getMusicEnabled());
                audioManager.playSound(SoundType::CLICK);
            } else if (mouseX >= 805 && mouseX <= 1030 && mouseY >= 340 && mouseY <= 458) {
                audioManager.setSoundEnabled(!audioManager.getSoundEnabled());
                audioManager.playSound(SoundType::CLICK);
            }
        } else if (menuState == MenuState::NEW_PANEL) {
            int backW, backH;
            SDL_QueryTexture(backTexture, nullptr, nullptr, &backW, &backH);
            if (mouseX >= 50 && mouseX <= 50 + backW && mouseY >= 50 && mouseY <= 50 + backH) {
                if (!stateHistory.empty()) {
                    auto prevState = stateHistory.top();
                    stateHistory.pop();
                    if (auto* prevMenuState = std::get_if<MenuState>(&prevState)) {
                        menuState = *prevMenuState;
                        menuVisible = true;
                    } else if (auto* prevGameState = std::get_if<GameState>(&prevState)) {
                        gameState = *prevGameState;
                        menuState = MenuState::NONE;
                        menuVisible = false;
                    }
                } else {
                    gameState = GameState::HOMEPLAY;
                    menuState = MenuState::NONE;
                    menuVisible = false;
                }
                audioManager.playSound(SoundType::CLICK);
            }
        } else if (menuState == MenuState::REVIVE && gameState == GameState::GAME_OVER && isReviveCountingDown) {
            int reviveW, reviveH;
            SDL_QueryTexture(reviveTexture, nullptr, nullptr, &reviveW, &reviveH);
            if (mouseX >= 570 && mouseX <= 950 && mouseY >= 445 && mouseY <= 625) {
                gameLoop.revivePlayer();
                gameState = GameState::PLAYING;
                menuState = MenuState::NONE;
                isPaused = false;
                isOffButtonActive = true;
                isReviveCountingDown = false;
                audioManager.playSound(SoundType::CLICK);
            }
        }
        if (menuState == MenuState::MENU && menuVisible) {
            if (mouseX >= 645 && mouseX <= 750 && mouseY >= 240 && mouseY <= 360) {
                gameState = GameState::HOMEPLAY;
                menuState = MenuState::NONE;
                menuVisible = false;
                isPaused = false;
                isOffButtonActive = true;
                audioManager.playSound(SoundType::CLICK);
                audioManager.playMusic(MusicType::HOMEPLAY);
                gameLoop.reset();
                while (!stateHistory.empty()) {
                    stateHistory.pop();
                }
            } else if (mouseX >= 230 && mouseX <= 615 && mouseY >= 545 && mouseY <= 690) {
                gameLoop.reset();
                gameState = GameState::PLAYING;
                menuState = MenuState::NONE;
                menuVisible = false;
                isPaused = false;
                isOffButtonActive = true;
                audioManager.playSound(SoundType::CLICK);
                audioManager.playMusic(MusicType::LOOP);
                while (!stateHistory.empty()) {
                    stateHistory.pop();
                }
            }
        }
    }
}

void Menu::render(Graphics& graphics, GameState gameState, MenuState menuState, bool& isPaused) {
    if (gameState == GameState::HOMEPLAY) {
        graphics.renderTexture(homeplayTexture, 0, 0);
        if (menuState == MenuState::MENU && menuVisible) {
            graphics.renderTexture(menuTexture, 0, 170);
            if (recordFont) {
                SDL_Color white = {255, 255, 255, 255};
                SDL_Rect size = {0, 0, 0, 0};
                std::string totalBananasText = std::to_string(gameLoop.getTotalBananas());
                SDL_Texture* totalBananasTexture = createTextTexture(graphics.getRenderer(), totalBananasText.c_str(), recordFont, white, size);
                if (totalBananasTexture) {
                    graphics.renderTexture(totalBananasTexture, 1030, 285);
                    SDL_DestroyTexture(totalBananasTexture);
                }
            }
        } else if (menuState == MenuState::OPTIONS) {
            graphics.renderTexture(optionsTexture, 300, 210);
            graphics.renderTexture(backTexture, 50, 50);
            graphics.renderTexture(audioManager.getMusicEnabled() ? musicOnTexture : musicOffTexture, 458, 340);
            graphics.renderTexture(audioManager.getSoundEnabled() ? effectOnTexture : effectOffTexture, 805, 340);
        } else if (menuState == MenuState::NEW_PANEL) {
            graphics.renderTexture(recordTexture, 300, 210);
            graphics.renderTexture(backTexture, 50, 50);

            if (recordFont) {
                SDL_Color white = {255, 255, 255, 255};
                SDL_Rect size = {0, 0, 0, 0};

                std::string bestScoreText = "Best Score: " + std::to_string(gameLoop.getBestBananas());
                SDL_Texture* bestScoreTexture = createTextTexture(graphics.getRenderer(), bestScoreText.c_str(), recordFont, white, size);
                if (bestScoreTexture) {
                    graphics.renderTexture(bestScoreTexture, 350, 300);
                    SDL_DestroyTexture(bestScoreTexture);
                }
                std::string currentScoreText = "Current Score: " + std::to_string(gameLoop.getLastGameBananas());
                SDL_Texture* currentScoreTexture = createTextTexture(graphics.getRenderer(), currentScoreText.c_str(), recordFont, white, size);
                if (currentScoreTexture) {
                    graphics.renderTexture(currentScoreTexture, 350, 360);
                    SDL_DestroyTexture(currentScoreTexture);
                }
            }
        }
    } else if (gameState == GameState::PLAYING || gameState == GameState::GAME_OVER) {
        if (menuState == MenuState::MENU && menuVisible) {
            graphics.renderTexture(menuTexture, 0, 170);
            graphics.renderTexture(onTexture, 1447, 0);
            if (recordFont) {
                SDL_Color white = {255, 255, 255, 255};
                SDL_Rect size = {0, 0, 0, 0};
                std::string totalBananasText = std::to_string(gameLoop.getTotalBananas());
                SDL_Texture* totalBananasTexture = createTextTexture(graphics.getRenderer(), totalBananasText.c_str(), recordFont, white, size);
                if (totalBananasTexture) {
                    graphics.renderTexture(totalBananasTexture, 1030, 285);
                    SDL_DestroyTexture(totalBananasTexture);
                }
            }
        } else if (menuState == MenuState::OPTIONS) {
            graphics.renderTexture(optionsTexture, 300, 210);
            graphics.renderTexture(backTexture, 50, 50);
            graphics.renderTexture(audioManager.getMusicEnabled() ? musicOnTexture : musicOffTexture, 458, 340);
            graphics.renderTexture(audioManager.getSoundEnabled() ? effectOnTexture : effectOffTexture, 805, 340);
        } else if (menuState == MenuState::NEW_PANEL) {
            graphics.renderTexture(recordTexture, 300, 210);
            graphics.renderTexture(backTexture, 50, 50);

            if (recordFont) {
                SDL_Color white = {255, 255, 255, 255};
                SDL_Rect size = {0, 0, 0, 0};

                std::string bestScoreText = "Best Score: " + std::to_string(gameLoop.getBestBananas());
                SDL_Texture* bestScoreTexture = createTextTexture(graphics.getRenderer(), bestScoreText.c_str(), recordFont, white, size);
                if (bestScoreTexture) {
                    graphics.renderTexture(bestScoreTexture, 350, 300);
                    SDL_DestroyTexture(bestScoreTexture);
                }

                std::string currentScoreText = "Current Score: " + std::to_string(gameLoop.getLastGameBananas());
                SDL_Texture* currentScoreTexture = createTextTexture(graphics.getRenderer(), currentScoreText.c_str(), recordFont, white, size);
                if (currentScoreTexture) {
                    graphics.renderTexture(currentScoreTexture, 350, 360);
                    SDL_DestroyTexture(currentScoreTexture);
                }
            }
        } else if (menuState == MenuState::REVIVE && gameState == GameState::GAME_OVER && isReviveCountingDown) {
            int reviveW, reviveH;
            SDL_QueryTexture(reviveTexture, nullptr, nullptr, &reviveW, &reviveH);
            int reviveX = (1500 - reviveW) / 2;
            int reviveY = (963 - reviveH) / 2;
            graphics.renderTexture(reviveTexture, reviveX, reviveY);

            Uint32 currentTime = SDL_GetTicks();
            float elapsedTime = (currentTime - reviveCountdownStartTime) / 1000.0f;
            int remainingTime = static_cast<int>(ceil(reviveCountdownValue - elapsedTime));

            if (reviveCountdownFont) {
                std::string text = std::to_string(remainingTime);
                SDL_Color white = {255, 255, 255, 255};
                SDL_Rect size = {0, 0, 0, 0};
                SDL_Texture* countdownTexture = createTextTexture(graphics.getRenderer(), text.c_str(), reviveCountdownFont, white, size);
                if (countdownTexture) {
                    graphics.renderTexture(countdownTexture, reviveX + 38, reviveY + 28);
                    SDL_DestroyTexture(countdownTexture);
                }
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
                    menuState = MenuState::NONE;
                    graphics.renderTexture(offTexture, 1447, 0);
                    return;
                }
                audioManager.playSound(SoundType::COUNT_DOWN);
            }

            if (countdownFont) {
                std::string text = std::to_string(countdownValue);
                SDL_Color black = {0, 0, 0, 255};
                SDL_Rect size = {0, 0, 0, 0};
                SDL_Texture* countdownTexture = createTextTexture(graphics.getRenderer(), text.c_str(), countdownFont, black, size);
                if (countdownTexture) {
                    SDL_Rect dstRect = {800 - size.w / 2, 400 - size.h / 2, size.w, size.h};
                    graphics.renderTexture(countdownTexture, dstRect.x, dstRect.y);
                    SDL_DestroyTexture(countdownTexture);
                }
            }
        } else if ((gameState == GameState::PLAYING || gameState == GameState::GAME_OVER) && menuState == MenuState::NONE) {
            graphics.renderTexture(offTexture, 1447, 0);
        }
    }
}
