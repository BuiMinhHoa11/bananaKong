#include "menu.h"
#include "gameloop.h"
#include <SDL_ttf.h>

Menu::Menu(Graphics& graphics, GameLoop& gameLoop) : menuVisible(false), gameLoop(gameLoop), isOffButtonActive(true), isCountingDown(false), countdownValue(0) {
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
}

Menu::~Menu() {
    SDL_DestroyTexture(homeplayTexture);
    SDL_DestroyTexture(menuTexture);
    SDL_DestroyTexture(offTexture);
    SDL_DestroyTexture(onTexture);
    SDL_DestroyTexture(optionsTexture);
    SDL_DestroyTexture(backTexture);
}

void Menu::togglePause(GameState& gameState, MenuState& menuState, bool& isPaused) {
    if (isOffButtonActive) {
        isPaused = true;
        menuVisible = true;
        isOffButtonActive = false;
        stateHistory.push_back(gameState);
        menuState = MENU;
    } else {
        menuVisible = false;
        startCountdown();
    }
}

void Menu::startCountdown() {
    isCountingDown = true;
    countdownValue = 3;
    countdownStartTime = SDL_GetTicks();
}

SDL_Texture* Menu::createCountdownTexture(Graphics& graphics, int value) {
    TTF_Font* font = TTF_OpenFont("D:/projectBTL/bananakong/font/Drawing_Kids.ttf", 100);
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
            } else if (mouseX >= 1346 && mouseX <= 1500 && mouseY >= 0 && mouseY <= 963) {
                SDL_Log("Opening menu in HOMEPLAY: mouseX=%d, mouseY=%d", mouseX, mouseY);
                stateHistory.push_back(gameState);
                menuVisible = true;
                menuState = MENU;
            }
        } else if (gameState == PLAYING && menuState == NONE && !isCountingDown) {
            if (mouseX >= 1447 && mouseX <= 1500 && mouseY >= 0 && mouseY <= 53) {
                togglePause(gameState, menuState, isPaused);
            }
        } else if (menuState == MENU && menuVisible) {
            if (mouseX >= 780 && mouseX <= 1197 && mouseY >= 408 && mouseY <= 545) {
                SDL_Log("Switching to OPTIONS: mouseX=%d, mouseY=%d", mouseX, mouseY);
                stateHistory.push_back(menuState);
                menuState = OPTIONS;
            } else if (mouseX < 1447 || mouseX > 1500 || mouseY < 0 || mouseY > 53) {
                menuVisible = false;
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
            }
        } else if (gameState == GAME_OVER) {
            gameLoop.reset();
            gameState = PLAYING;
            menuState = NONE;
            isPaused = false;
            isOffButtonActive = true;
            isCountingDown = false;
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
                    return;
                }
            }
            SDL_Texture* countdownTexture = createCountdownTexture(graphics, countdownValue);
            if (countdownTexture) {
                int w, h;
                SDL_QueryTexture(countdownTexture, nullptr, nullptr, &w, &h);
                SDL_Rect dstRect = {800 - w / 2, 400 - h / 2, w, h};
                graphics.renderTexture(countdownTexture, dstRect.x, dstRect.y);
                SDL_DestroyTexture(countdownTexture);
            }
        } else if (gameState == PLAYING && menuState == NONE && !isCountingDown) {
            graphics.renderTexture(offTexture, 1447, 0);
        }
    }
}
