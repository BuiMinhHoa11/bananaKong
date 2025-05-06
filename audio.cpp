#include "audio.h"
#include <SDL.h>
#include <iostream>

AudioManager::AudioManager() : musicEnabled(true), soundEnabled(true), currentMusic(MusicType::HOMEPLAY) {}

AudioManager::~AudioManager() {
    for (auto& [type, sound] : soundEffects) {
        Mix_FreeChunk(sound);
    }
    for (auto& [type, music] : backgroundMusic) {
        Mix_FreeMusic(music);
    }
    Mix_CloseAudio();
}

bool AudioManager::init() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Failed to initialize SDL_mixer: " << Mix_GetError() << std::endl;
        return false;
    }
    return true;
}

void AudioManager::loadSounds() {
    soundEffects[SoundType::DIE] = Mix_LoadWAV("D:/projectBTL/bananakong/audio/incidental_music/die.mp3");
    soundEffects[SoundType::COUNT_DOWN] = Mix_LoadWAV("D:/projectBTL/bananakong/audio/incidental_music/count_down.mp3");
    soundEffects[SoundType::EAT] = Mix_LoadWAV("D:/projectBTL/bananakong/audio/sound_effect/eat.mp3");
    soundEffects[SoundType::JUMP] = Mix_LoadWAV("D:/projectBTL/bananakong/audio/sound_effect/jump.mp3");
    soundEffects[SoundType::CLIMB_DOWN] = Mix_LoadWAV("D:/projectBTL/bananakong/audio/sound_effect/climb_down.mp3");
    soundEffects[SoundType::SLIDE] = Mix_LoadWAV("D:/projectBTL/bananakong/audio/sound_effect/slide.mp3");
    soundEffects[SoundType::CLICK] = Mix_LoadWAV("D:/projectBTL/bananakong/audio/sound_effect/click.mp3");
    soundEffects[SoundType::RUN] = Mix_LoadWAV("D:/projectBTL/bananakong/audio/sound_effect/run.mp3");

    backgroundMusic[MusicType::HOMEPLAY] = Mix_LoadMUS("D:/projectBTL/bananakong/audio/incidental_music/homeplay_music.mp3");
    backgroundMusic[MusicType::LOOP] = Mix_LoadMUS("D:/projectBTL/bananakong/audio/incidental_music/loop_music.mp3");

    for (const auto& [type, sound] : soundEffects) {
        if (!sound) {
            std::cerr << "Failed to load sound effect: " << Mix_GetError() << std::endl;
        }
    }
    for (const auto& [type, music] : backgroundMusic) {
        if (!music) {
            std::cerr << "Failed to load incidental music: " << Mix_GetError() << std::endl;
        }
    }
}

void AudioManager::playSound(SoundType type) {
    if (soundEnabled && soundEffects.find(type) != soundEffects.end() && soundEffects[type]) {
        Mix_PlayChannel(-1, soundEffects[type], 0);
    }
}

void AudioManager::playMusic(MusicType type) {
    if (musicEnabled && backgroundMusic.find(type) != backgroundMusic.end() && backgroundMusic[type]) {
        if (currentMusic != type || !Mix_PlayingMusic()) {
            Mix_HaltMusic();
            Mix_PlayMusic(backgroundMusic[type], -1);
            currentMusic = type;
        }
    }
}

void AudioManager::stopMusic() {
    Mix_HaltMusic();
}

void AudioManager::setMusicEnabled(bool enabled) {
    musicEnabled = enabled;
    if (!musicEnabled) {
        Mix_HaltMusic();
    } else {
        playMusic(currentMusic);
    }
}

void AudioManager::setSoundEnabled(bool enabled) {
    soundEnabled = enabled;
}
