#pragma once

#include "StaticSprite.h"
#include "Texture.h"
#include "TexturedQuad.h"
#include "IGameScene.h"

enum class CreditsMenuSelection 
{
    None = -1,
    Back = 0
};

class CreditsMenu : public IGameScene
{
public:
    CreditsMenu(int width, int height);
    ~CreditsMenu();

    int update(int deltaTime) override;
    void render() override;

private:
    Texture instructionsTexture;

    TexturedQuad *quad;
    ShaderProgram texProgram;
    glm::mat4 projection;
};