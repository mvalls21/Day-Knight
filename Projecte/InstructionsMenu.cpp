#include "InstructionsMenu.h"

#include "Shader.h"
#include <iostream>

#include "Game.h"
#include <GL/glew.h>
#include <GL/glut.h>

#include <glm/gtc/matrix_transform.hpp>

InstructionsMenu::InstructionsMenu(int width, int height)
{
    instructionsTexture.loadFromFile("images/main_menu/instructions_menu.png", PixelFormat::TEXTURE_PIXEL_FORMAT_RGBA);

    projection = glm::ortho(0.f, float(width - 1), float(height - 1), 0.f);

    {
        Shader vShader, fShader;

        vShader.initFromFile(VERTEX_SHADER, "shaders/texture.vert");
        if (!vShader.isCompiled())
        {
            cout << "Vertex Shader Error" << endl;
            cout << "" << vShader.log() << endl
                 << endl;
        }
        fShader.initFromFile(FRAGMENT_SHADER, "shaders/texture.frag");
        if (!fShader.isCompiled())
        {
            cout << "Fragment Shader Error" << endl;
            cout << "" << fShader.log() << endl
                 << endl;
        }
        texProgram.init();
        texProgram.addShader(vShader);
        texProgram.addShader(fShader);
        texProgram.link();
        if (!texProgram.isLinked())
        {
            cout << "Shader Linking Error" << endl;
            cout << "" << texProgram.log() << endl
                 << endl;
        }
        texProgram.bindFragmentOutput("outColor");
        vShader.free();
        fShader.free();
    }

    // quad = StaticSprite::createSprite({width, height}, {1.0f, 1.0f}, &selectedPlay, &texProgram);

    glm::vec2 geom[] = {
        {0.0f, 0.0f},
        {width, height}};

    glm::vec2 texCoords[] = {
        {0.0f, 0.0f},
        {1.0f, 1.0f}};

    quad = TexturedQuad::createTexturedQuad(geom, texCoords, texProgram);
}

InstructionsMenu::~InstructionsMenu()
{
}

void InstructionsMenu::render()
{
    texProgram.use();
    texProgram.setUniformMatrix4f("projection", projection);
    texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
    glm::mat4 modelview(1.0f);
    texProgram.setUniformMatrix4f("modelview", modelview);
    texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);

    quad->render(instructionsTexture);
}

int InstructionsMenu::update(int deltaTime)
{
    if (Game::instance().getKey(27)) // esc key
        return (int)InstructionsMenuSelection::Back;

    return None;
}