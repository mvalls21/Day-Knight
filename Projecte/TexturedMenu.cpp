#include "TexturedMenu.h"

#include "Shader.h"
#include <iostream>
#include "ShaderSystem.h"

#include "Game.h"
#include <GL/glew.h>
#include <GL/glut.h>

#include <glm/gtc/matrix_transform.hpp>

TexturedMenu::TexturedMenu(int width, int height, const std::string& texturePath)
{
    texture.loadFromFile(texturePath, PixelFormat::TEXTURE_PIXEL_FORMAT_RGBA);

    projection = glm::ortho(0.f, float(width - 1), float(height - 1), 0.f);

    texProgram = ShaderSystem::acquire("texture");

    glm::vec2 geom[] = {
        {0.0f, 0.0f},
        {width, height}};

    glm::vec2 texCoords[] = {
        {0.0f, 0.0f},
        {1.0f, 1.0f}};

    quad = TexturedQuad::createTexturedQuad(geom, texCoords, *texProgram);
}

TexturedMenu::~TexturedMenu()
{
}

void TexturedMenu::render()
{
    texProgram->use();
    texProgram->setUniformMatrix4f("projection", projection);
    texProgram->setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
    glm::mat4 modelview(1.0f);
    texProgram->setUniformMatrix4f("modelview", modelview);
    texProgram->setUniform2f("texCoordDispl", 0.f, 0.f);

    quad->render(texture);
}

int TexturedMenu::update(int deltaTime)
{
    if (Game::instance().getKey(27)) // esc key
        return (int)TexturedMenuSelection::Back;

    return (int)TexturedMenuSelection::None;
}