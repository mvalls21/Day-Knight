#pragma once

#include <irrKlang.h>

class SoundManager {
public:
    static SoundManager getManager();

    void playSound(const char* path);
    void playStackableSound(const char* path);
    void playSoundtrack(const char* path);
    void stopAllSounds();

private:
    SoundManager();
    irrklang::ISoundEngine *engine;
};