#include "MainMenu.h"

#include "Shader.h"
#include <iostream>

#include "Game.h"
#include <GL/glew.h>
#include <GL/glut.h>

#include <glm/gtc/matrix_transform.hpp>

MainMenu::MainMenu(int width, int height)
{
    selectedPlay.loadFromFile("images/main_menu/selected_play.png", PixelFormat::TEXTURE_PIXEL_FORMAT_RGBA);
    selectedCredits.loadFromFile("images/main_menu/selected_credits.png", PixelFormat::TEXTURE_PIXEL_FORMAT_RGBA);
    selectedInstructions.loadFromFile("images/main_menu/selected_instructions.png", PixelFormat::TEXTURE_PIXEL_FORMAT_RGBA);

    projection = glm::ortho(0.f, float(width - 1), float(height - 1), 0.f);

    // glm::vec2 geom[] = {
    //     {0.0f, 0.0f},
    //     {width, height}};
    // glm::vec2 texCoords[] = {
    //     {0.0f, 0.0f},
    //     {1.0f, 1.0f}};

    // quad = TexturedQuad::createTexturedQuad(geom, texCoords, texProgram);

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

MainMenu::~MainMenu()
{
}

void MainMenu::render()
{
    texProgram.use();
    texProgram.setUniformMatrix4f("projection", projection);
    texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
    glm::mat4 modelview(1.0f);
    texProgram.setUniformMatrix4f("modelview", modelview);
    texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);

    if (current == 0)
        quad->render(selectedPlay);
    if (current == 1)
        quad->render(selectedCredits);
    if (current == 2)
        quad->render(selectedInstructions);
}

void MainMenu::update(int deltaTime)
{
    time += deltaTime;
    if (time <= 200)
        return;

    if (Game::instance().getSpecialKey(GLUT_KEY_DOWN))
    {
        time = 0;
        current = (current + 1) % 3;
    }

    if (Game::instance().getSpecialKey(GLUT_KEY_UP))
    {
        time = 0;
        current--;
        if (current == -1) current = 2;
    }
}