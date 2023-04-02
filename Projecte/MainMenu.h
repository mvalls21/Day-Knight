#pragma once

#include "StaticSprite.h"
#include "Texture.h"
#include "TexturedQuad.h"
#include "IGameScene.h"

enum MainMenuSelection
{
    None = -1,
    Play = 0,
    Credits = 1,
    Instructions = 2,
    Exit = 3
};

class MainMenu : public IGameScene
{
public:
    MainMenu(int width, int height);
    ~MainMenu();

    int update(int deltaTime) override;
    void render() override;

private:
    Texture selectedPlay;
    Texture selectedCredits;
    Texture selectedInstructions;
    Texture selectedExit;

    TexturedQuad *quad;
    ShaderProgram* texProgram;
    glm::mat4 projection;

    int selection;
    int time = 0;
};