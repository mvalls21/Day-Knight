#include "Skeleton.h"

#include <GL/glew.h>
#include <GL/glut.h>
#include <cmath>
#include <iostream>

#include "Game.h"

void Skeleton::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram)
{
    spritesheet.loadFromFile("images/skeleton.png", TEXTURE_PIXEL_FORMAT_RGBA);

    sprite = AnimatedSprite::createSprite(glm::ivec2(32), glm::vec2(1.0f / 4.0f, 1.0f / 3.0f), &spritesheet, &shaderProgram);
    sprite->setNumberAnimations(4);

    sprite->setAnimationSpeed(STAND_LEFT, 8);
    sprite->addKeyframe(STAND_LEFT, glm::vec2(0.f, 0.f));

    sprite->setAnimationSpeed(STAND_RIGHT, 8);
    sprite->addKeyframe(STAND_RIGHT, glm::vec2(0.25f, 0.f));

    sprite->setAnimationSpeed(MOVE_LEFT, 8);
    sprite->addKeyframe(MOVE_LEFT, glm::vec2(3.0f / 4.0f, 1.0f / 3.0f));
    sprite->addKeyframe(MOVE_LEFT, glm::vec2(2.0f / 4.0f, 1.0f / 3.0f));
    sprite->addKeyframe(MOVE_LEFT, glm::vec2(1.0f / 4.0f, 1.0f / 3.0f));
    sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.0f / 4.0f, 1.0f / 3.0f));

    sprite->setAnimationSpeed(MOVE_RIGHT, 8);
    sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.0f / 4.0f, 0.f));
    sprite->addKeyframe(MOVE_RIGHT, glm::vec2(1.0f / 4.0f, 0.0f));
    sprite->addKeyframe(MOVE_RIGHT, glm::vec2(2.0f / 4.0f, 0.0f));
    sprite->addKeyframe(MOVE_RIGHT, glm::vec2(3.0f / 4.0f, 0.0f));

    sprite->changeAnimation(MOVE_RIGHT);
    tileMapDispl = tileMapPos;
    sprite->setPosition(glm::vec2(float(tileMapDispl.x + position.x), float(tileMapDispl.y + position.y)));
}
