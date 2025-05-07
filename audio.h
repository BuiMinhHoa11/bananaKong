#ifndef AUDIO_H
#define AUDIO_H

#include <SDL_mixer.h>
#include <map>
#include <string>

enum class SoundType {
    COUNT_DOWN,
    EAT,
    JUMP,
    CLIMB_DOWN,
    SLIDE,
    CLICK,
    DIE_EFFECT
};

enum class MusicType {
    HOMEPLAY,
    LOOP
};

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    bool init();
    void loadSounds();
    void playSound(SoundType type);
    void playMusic(MusicType type);
    void stopMusic();
    void setMusicEnabled(bool enabled);
    void setSoundEnabled(bool enabled);
    bool getMusicEnabled() const { return musicEnabled; }
    bool getSoundEnabled() const { return soundEnabled; }

private:
    std::map<SoundType, Mix_Chunk*> soundEffects;
    std::map<MusicType, Mix_Music*> backgroundMusic;
    bool musicEnabled;
    bool soundEnabled;
    MusicType currentMusic;
};

#endif // AUDIO_H
