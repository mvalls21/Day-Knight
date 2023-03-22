#pragma once

#include "Text.h"
#include "Scene.h"

class LevelSelection
{
public:
    LevelSelection();
    ~LevelSelection();

    void render() const;

private:
    Text* text;
    std::vector<Scene*> levels;

    void drawTitle();
    void drawLevels();
    void drawBack();
};