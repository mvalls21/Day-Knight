#include <string>
#include "SoundManager.h"

SoundManager::SoundManager()
{
    engine = irrklang::createIrrKlangDevice();
    if (engine == nullptr)
        abort();
}

SoundManager SoundManager::getManager()
{
    static SoundManager manager;
    return manager;
}

void SoundManager::playStackableSound(const char* path)
{
    engine->play2D(path, false);
}

void SoundManager::playSound(const char* path)
{
    if (not engine->isCurrentlyPlaying(path))
    {
        playStackableSound(path);
    }
}

void SoundManager::playSoundtrack(const char* path)
{
    if (not engine->isCurrentlyPlaying(path))
    {
        stopAllSounds();
        engine->play2D(path, true);
    }
}

void SoundManager::stopAllSounds()
{
    engine->stopAllSounds();
}
