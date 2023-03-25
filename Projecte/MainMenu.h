#pragma once

#include "StaticSprite.h"
#include "Texture.h"
#include "TexturedQuad.h"

enum MainMenuSelection
{
    None = -1,
    Play = 0,
    Credits = 1,
    Instructions = 2,
    Exit = 3
};

class MainMenu
{
public:
    MainMenu(int width, int height);
    ~MainMenu();

    void render();
    int update(int deltaTime);

private:
    Texture selectedPlay;
    Texture selectedCredits;
    Texture selectedInstructions;
    Texture selectedExit;

    TexturedQuad *quad;
    ShaderProgram texProgram;
    glm::mat4 projection;

    int selection;
    int time = 0;
};