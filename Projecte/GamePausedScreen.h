#pragma once

#include "StaticSprite.h"
#include "Texture.h"
#include "TexturedQuad.h"
#include "IGameScene.h"

enum class GamePausedSelection
{
    None = -1,
    Continue = 0,
    Restart = 1,
    ExitMainMenu = 2
};

class GamePausedScreen : public IGameScene
{
public:
    GamePausedScreen(int width, int height);
    ~GamePausedScreen();

    int update(int deltaTime) override;
    void render() override;

private:
    Texture selectedContinue;
    Texture selectedRestart;
    Texture selectedExitMainMenu;

    TexturedQuad *quad;
    ShaderProgram *texProgram;
    glm::mat4 projection;

    int selection;
    int time = 0;
};