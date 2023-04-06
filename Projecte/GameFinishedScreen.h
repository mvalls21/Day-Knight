#pragma once

#include "StaticSprite.h"
#include "Texture.h"
#include "TexturedQuad.h"
#include "IGameScene.h"
#include "AnimatedSprite.h"
#include "Text.h"

enum class GameFinishedSelection 
{
    None = -1,
    Credits = 0,
    MainMenu = 1,
};

class GameFinishedScreen : public IGameScene
{
public:
    GameFinishedScreen(int width, int height, int* score);
    ~GameFinishedScreen();

    int update(int deltaTime) override;
    void render() override;

private:
    Texture selectedCredits;
    Texture selectedMainMenu;

    TexturedQuad *quad;
    ShaderProgram *texProgram;
    glm::mat4 projection;

    Text* text;

    AnimatedSprite* sprite;
    Texture* spritesheet;

    int selection;
    int time = 0;

    int* score;
};