#include "GameFinishedScreen.h"

#include "Shader.h"
#include <iostream>

#include "ShaderSystem.h"

#include "Game.h"
#include <GL/glew.h>
#include <GL/glut.h>

#include <glm/gtc/matrix_transform.hpp>

GameFinishedScreen::GameFinishedScreen(int width, int height, int *score)
{
    selectedCredits.loadFromFile("images/game_finished/game_finished_selected_credits.png", PixelFormat::TEXTURE_PIXEL_FORMAT_RGBA);
    selectedMainMenu.loadFromFile("images/game_finished/game_finished_selected_main_menu.png", PixelFormat::TEXTURE_PIXEL_FORMAT_RGBA);

    this->score = score;

    text = new Text();
    if (!text->init("../Projecte/fonts/OldLondon.ttf"))
        abort();

    projection = glm::ortho(0.f, float(width - 1), float(height - 1), 0.f);

    selection = (int)GameFinishedSelection::Credits;

    texProgram = ShaderSystem::acquire("texture");

    glm::vec2 geom[] = {
        {0.0f, 0.0f},
        {width, height}};

    glm::vec2 texCoords[] = {
        {0.0f, 0.0f},
        {1.0f, 1.0f}};

    quad = TexturedQuad::createTexturedQuad(geom, texCoords, *texProgram);

    spritesheet = new Texture();
    spritesheet->loadFromFile("images/player_game_finished.png", PixelFormat::TEXTURE_PIXEL_FORMAT_RGBA);

    sprite = AnimatedSprite::createSprite(glm::vec2(192), glm::vec2(1.0f / 9.0f, 1.0f / 3.0f), spritesheet, texProgram);
    sprite->setPosition(glm::vec2(width - 192.0f * 2.0f, height - 192.0f * 2.0f));

    sprite->setNumberAnimations(1);

    sprite->setAnimationSpeed(0, 8);
    sprite->addKeyframe(0, glm::vec2(0.0f / 9.0f, 2.0f / 3.0f));
    sprite->addKeyframe(0, glm::vec2(1.0f / 9.0f, 2.0f / 3.0f));
    sprite->addKeyframe(0, glm::vec2(2.0f / 9.0f, 2.0f / 3.0f));
    sprite->addKeyframe(0, glm::vec2(3.0f / 9.0f, 2.0f / 3.0f));
    sprite->addKeyframe(0, glm::vec2(4.0f / 9.0f, 2.0f / 3.0f));
    sprite->addKeyframe(0, glm::vec2(5.0f / 9.0f, 2.0f / 3.0f));

    sprite->addKeyframe(0, glm::vec2(0.0f / 9.0f, 0.0f / 3.0f));
    sprite->addKeyframe(0, glm::vec2(1.0f / 9.0f, 0.0f / 3.0f));
    sprite->addKeyframe(0, glm::vec2(2.0f / 9.0f, 0.0f / 3.0f));
    sprite->addKeyframe(0, glm::vec2(3.0f / 9.0f, 0.0f / 3.0f));
    sprite->addKeyframe(0, glm::vec2(4.0f / 9.0f, 0.0f / 3.0f));
    sprite->addKeyframe(0, glm::vec2(5.0f / 9.0f, 0.0f / 3.0f));
    sprite->addKeyframe(0, glm::vec2(6.0f / 9.0f, 0.0f / 3.0f));
    sprite->addKeyframe(0, glm::vec2(7.0f / 9.0f, 0.0f / 3.0f));
    sprite->addKeyframe(0, glm::vec2(8.0f / 9.0f, 0.0f / 3.0f));

    sprite->addKeyframe(0, glm::vec2(0.0f / 9.0f, 1.0f / 3.0f));
    sprite->addKeyframe(0, glm::vec2(1.0f / 9.0f, 1.0f / 3.0f));
    sprite->addKeyframe(0, glm::vec2(2.0f / 9.0f, 1.0f / 3.0f));
    sprite->addKeyframe(0, glm::vec2(3.0f / 9.0f, 1.0f / 3.0f));
    sprite->addKeyframe(0, glm::vec2(4.0f / 9.0f, 1.0f / 3.0f));
    sprite->addKeyframe(0, glm::vec2(5.0f / 9.0f, 1.0f / 3.0f));
    sprite->addKeyframe(0, glm::vec2(6.0f / 9.0f, 1.0f / 3.0f));
    sprite->addKeyframe(0, glm::vec2(7.0f / 9.0f, 1.0f / 3.0f));
    sprite->addKeyframe(0, glm::vec2(8.0f / 9.0f, 1.0f / 3.0f));

    sprite->changeAnimation(0);
}

GameFinishedScreen::~GameFinishedScreen()
{
    delete sprite;
    delete spritesheet;
}

void GameFinishedScreen::render()
{
    texProgram->use();
    texProgram->setUniformMatrix4f("projection", projection);
    texProgram->setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
    glm::mat4 modelview(1.0f);
    texProgram->setUniformMatrix4f("modelview", modelview);
    texProgram->setUniform2f("texCoordDispl", 0.f, 0.f);

    switch (static_cast<GameFinishedSelection>(selection))
    {
    case GameFinishedSelection::Credits:
        quad->render(selectedCredits);
        break;
    case GameFinishedSelection::MainMenu:
        quad->render(selectedMainMenu);
        break;
    }

    const std::string scoreText = "You finished with a score of:   " + std::to_string(static_cast<int>(*score));
    
    text->render(scoreText, glm::vec2(SCREEN_WIDTH / 2 - 450, SCREEN_HEIGHT / 2 - 75), 70, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    sprite->render();
}

int GameFinishedScreen::update(int deltaTime)
{
    time += deltaTime;

    sprite->update(deltaTime);

    if (time <= 200)
        return None;

    if (Game::instance().getKey(13)) // 13 is enter key
    {
        return selection;
    }

    if (Game::instance().getSpecialKey(GLUT_KEY_DOWN))
    {
        time = 0;
        selection = (selection + 1) % 2;
    }

    if (Game::instance().getSpecialKey(GLUT_KEY_UP))
    {
        time = 0;
        selection--;
        if (selection == -1)
            selection = (int)GameFinishedSelection::MainMenu;
    }

    return None;
}