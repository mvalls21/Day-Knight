#pragma once

#include "StaticSprite.h"
#include "Texture.h"
#include "TexturedQuad.h"
#include "IGameScene.h"

enum class InstructionsMenuSelection 
{
    None = -1,
    Back = 0
};

class InstructionsMenu : public IGameScene
{
public:
    InstructionsMenu(int width, int height);
    ~InstructionsMenu();

    int update(int deltaTime) override;
    void render() override;

private:
    Texture instructionsTexture;

    TexturedQuad *quad;
    ShaderProgram texProgram;
    glm::mat4 projection;
};