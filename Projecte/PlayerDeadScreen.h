#pragma once

#include "StaticSprite.h"
#include "Texture.h"
#include "TexturedQuad.h"
#include "IGameScene.h"

enum class PlayerDeadSelection
{
    None = -1,
    StartAgain = 0,
    ExitMainMenu = 1,
    ExitDesktop = 2
};

enum class ReasonLostGame
{
    PlayerDead,
    TimeIsUp
};

class PlayerDeadScreen : public IGameScene
{
public:
    PlayerDeadScreen(int width, int height, ReasonLostGame reason);
    ~PlayerDeadScreen();

    int update(int deltaTime) override;
    void render() override;

private:
    Texture selectedPlayAgain;
    Texture selectedExitMainMenu;
    Texture selectedExitDesktop;

    TexturedQuad *quad;
    ShaderProgram *texProgram;
    glm::mat4 projection;

    int selection;
    int time = 0;
};