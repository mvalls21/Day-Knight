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

void SoundManager::playSound(const char* path)
{
    engine->play2D(path, false);
}

void SoundManager::playSoundtrack(const char* path)
{
    engine->play2D(path, true);
}

void SoundManager::stopAllSounds()
{
    engine->stopAllSounds();
}
