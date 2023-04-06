#include "PlayerDeadScreen.h"

#include "Shader.h"
#include <iostream>

#include "ShaderSystem.h"

#include "Game.h"
#include <GL/glew.h>
#include <GL/glut.h>

#include <glm/gtc/matrix_transform.hpp>
#include "SoundManager.h"

PlayerDeadScreen::PlayerDeadScreen(int width, int height)
{
    selectedPlayAgain.loadFromFile("images/dead_screen/dead_selected_start_again.png", PixelFormat::TEXTURE_PIXEL_FORMAT_RGBA);
    selectedExitMainMenu.loadFromFile("images/dead_screen/dead_selected_main_menu.png", PixelFormat::TEXTURE_PIXEL_FORMAT_RGBA);
    selectedExitDesktop.loadFromFile("images/dead_screen/dead_selected_desktop.png", PixelFormat::TEXTURE_PIXEL_FORMAT_RGBA);

    projection = glm::ortho(0.f, float(width - 1), float(height - 1), 0.f);

    selection = (int)PlayerDeadSelection::StartAgain;

    texProgram = ShaderSystem::acquire("texture");

    glm::vec2 geom[] = {
        {0.0f, 0.0f},
        {width, height}};

    glm::vec2 texCoords[] = {
        {0.0f, 0.0f},
        {1.0f, 1.0f}};

    quad = TexturedQuad::createTexturedQuad(geom, texCoords, *texProgram);
}

PlayerDeadScreen::~PlayerDeadScreen()
{
}

void PlayerDeadScreen::render()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    texProgram->use();
    texProgram->setUniformMatrix4f("projection", projection);
    texProgram->setUniform4f("color", 1.0f, 1.0f, 1.0f, 0.7f);
    glm::mat4 modelview(1.0f);
    texProgram->setUniformMatrix4f("modelview", modelview);
    texProgram->setUniform2f("texCoordDispl", 0.f, 0.f);

    switch (static_cast<PlayerDeadSelection>(selection))
    {
    case PlayerDeadSelection::StartAgain:
        quad->render(selectedPlayAgain);
        break;
    case PlayerDeadSelection::ExitMainMenu:
        quad->render(selectedExitMainMenu);
        break;
    case PlayerDeadSelection::ExitDesktop:
        quad->render(selectedExitDesktop);
        break;
    }
    glDisable(GL_BLEND);
}

int PlayerDeadScreen::update(int deltaTime)
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
        SoundManager::getManager().playStackableSound("sounds/menuItem.wav");

        time = 0;
        selection = (selection + 1) % 3;
    }

    if (Game::instance().getSpecialKey(GLUT_KEY_UP))
    {
        SoundManager::getManager().playStackableSound("sounds/menuItem.wav");

        time = 0;
        selection--;
        if (selection == -1)
            selection = (int)PlayerDeadSelection::ExitDesktop;
    }

    return None;
}