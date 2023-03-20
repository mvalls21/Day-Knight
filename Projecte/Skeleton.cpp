#include "Skeleton.h"

#include <GL/glew.h>
#include <GL/glut.h>
#include <cmath>
#include <iostream>

#include "Game.h"

void Skeleton::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram)
{
    // TODO: Change sprite
    spritesheet.loadFromFile("images/skeleton.png", TEXTURE_PIXEL_FORMAT_RGBA);

    sprite = AnimatedSprite::createSprite(glm::ivec2(32), glm::vec2(1.0f / 4.0f, 1.0f / 3.0f), &spritesheet, &shaderProgram);
    sprite->setNumberAnimations(4);

    // sprite->setAnimationSpeed(STAND_LEFT, 8);
    // sprite->addKeyframe(STAND_LEFT, glm::vec2(0.f, 0.f));

    // sprite->setAnimationSpeed(STAND_RIGHT, 8);
    // sprite->addKeyframe(STAND_RIGHT, glm::vec2(0.25f, 0.f));

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

void Skeleton::update(int deltaTime)
{
    sprite->update(deltaTime);

    position.x += movementSpeed;

    const glm::ivec2 tilePosition = position / map->getTileSize();

    if (tilePosition == tileEnd)
    {
        changeDirection();

        auto tmp = tileEnd;
        tileEnd = tileStart;
        tileStart = tmp;
    }

    sprite->setPosition(glm::vec2(float(tileMapDispl.x + position.x), float(tileMapDispl.y + position.y)));
}

void Skeleton::changeDirection()
{
    if (currentDirection == MOVE_LEFT)
    {
        currentDirection = MOVE_RIGHT;
    }
    else if (currentDirection == MOVE_RIGHT)
    {
        currentDirection = MOVE_LEFT;
    }

    sprite->changeAnimation(currentDirection);
    movementSpeed = -movementSpeed;
}

void Skeleton::setDirection(CharacterAnims direction)
{
    switch (direction)
    {
    case STAND_LEFT:
        currentDirection = MOVE_LEFT;
        break;
    case STAND_RIGHT:
        currentDirection = MOVE_RIGHT;
        break;
    case MOVE_LEFT:
    case MOVE_RIGHT:
        currentDirection = direction;
        break;
    }

    sprite->changeAnimation(currentDirection);

    if (currentDirection == MOVE_RIGHT)
        movementSpeed = SKELETON_MOVEMENT_SPEED;
    else
        movementSpeed = -SKELETON_MOVEMENT_SPEED;
}

void Skeleton::setMovementRange(const glm::ivec2 &tileStart, const glm::ivec2 &tileEnd)
{
    this->tileStart = tileStart;
    this->tileEnd = tileEnd;

    assert(tileStart.y == tileEnd.y);

    if (tileEnd.x > tileStart.x)
        setDirection(MOVE_RIGHT);
    else
        setDirection(MOVE_LEFT);

    setPosition({tileStart.x * map->getTileSize(), tileStart.y * map->getTileSize()});
}