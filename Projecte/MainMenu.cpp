#include "MainMenu.h"

#include "Shader.h"
#include <iostream>

#include "ShaderSystem.h"

#include "Game.h"
#include <GL/glew.h>
#include <GL/glut.h>

#include <glm/gtc/matrix_transform.hpp>

MainMenu::MainMenu(int width, int height)
{
    selectedPlay.loadFromFile("images/main_menu/selected_play.png", PixelFormat::TEXTURE_PIXEL_FORMAT_RGBA);
    selectedCredits.loadFromFile("images/main_menu/selected_credits.png", PixelFormat::TEXTURE_PIXEL_FORMAT_RGBA);
    selectedInstructions.loadFromFile("images/main_menu/selected_instructions.png", PixelFormat::TEXTURE_PIXEL_FORMAT_RGBA);
    selectedExit.loadFromFile("images/main_menu/selected_exit.png", PixelFormat::TEXTURE_PIXEL_FORMAT_RGBA);

    projection = glm::ortho(0.f, float(width - 1), float(height - 1), 0.f);

    selection = MainMenuSelection::Play;

    texProgram = ShaderSystem::acquire("texture");

    glm::vec2 geom[] = {
        {0.0f, 0.0f},
        {width, height}};

    glm::vec2 texCoords[] = {
        {0.0f, 0.0f},
        {1.0f, 1.0f}};

    quad = TexturedQuad::createTexturedQuad(geom, texCoords, *texProgram);
}

MainMenu::~MainMenu()
{
}

void MainMenu::render()
{
    texProgram->use();
    texProgram->setUniformMatrix4f("projection", projection);
    texProgram->setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
    glm::mat4 modelview(1.0f);
    texProgram->setUniformMatrix4f("modelview", modelview);
    texProgram->setUniform2f("texCoordDispl", 0.f, 0.f);

    switch (selection)
    {
    case MainMenuSelection::Play:
        quad->render(selectedPlay);
        break;
    case MainMenuSelection::Credits:
        quad->render(selectedCredits);
        break;
    case MainMenuSelection::Instructions:
        quad->render(selectedInstructions);
        break;
    case MainMenuSelection::Exit:
        quad->render(selectedExit);
        break;
    }
}

int MainMenu::update(int deltaTime)
{
    time += deltaTime;
    if (time <= 200)
        return None;

    if (Game::instance().getKey(13)) // 13 is enter key
    {
        return selection;
    }

    if (Game::instance().getSpecialKey(GLUT_KEY_DOWN))
    {
        time = 0;
        selection = (selection + 1) % 4;
    }

    if (Game::instance().getSpecialKey(GLUT_KEY_UP))
    {
        time = 0;
        selection--;
        if (selection == -1)
            selection = MainMenuSelection::Exit;
    }

    return None;
}