#pragma once

#include "Texture.h"
#include "TexturedQuad.h"
#include "IGameScene.h"

enum class TexturedMenuSelection
{
    None = -1,
    Back = 0
};

class TexturedMenu : public IGameScene
{
public:
    TexturedMenu(int width, int height, const std::string &texturePath);
    ~TexturedMenu();

    int update(int deltaTime) override;
    void render() override;

private:
    Texture texture;

    TexturedQuad *quad;
    ShaderProgram* texProgram;
    glm::mat4 projection;
};