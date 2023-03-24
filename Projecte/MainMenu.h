#pragma once

#include "StaticSprite.h"
#include "Texture.h"
#include "TexturedQuad.h"

class MainMenu
{
public:
    MainMenu(int width, int height);
    ~MainMenu();

    void render();
    void update(int deltaTime);

private:
    Texture selectedPlay;
    Texture selectedCredits;
    Texture selectedInstructions;

    TexturedQuad *quad;

    ShaderProgram texProgram;

    glm::mat4 projection;

    int current = 0;
    int time = 0;
};