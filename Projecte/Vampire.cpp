#include "Vampire.h"

#include <GL/glew.h>
#include <GL/glut.h>
#include <cmath>
#include <iostream>

#include "Game.h"

void Vampire::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram)
{
    spritesheet.loadFromFile("images/bub.png", TEXTURE_PIXEL_FORMAT_RGBA);

    sprite = AnimatedSprite::createSprite(glm::ivec2(32), glm::vec2(0.25, 0.25), &spritesheet, &shaderProgram);
    sprite->setNumberAnimations(4);

    sprite->setAnimationSpeed(STAND_LEFT, 8);
    sprite->addKeyframe(STAND_LEFT, glm::vec2(0.f, 0.f));

    sprite->setAnimationSpeed(STAND_RIGHT, 8);
    sprite->addKeyframe(STAND_RIGHT, glm::vec2(0.25f, 0.f));

    sprite->setAnimationSpeed(MOVE_LEFT, 8);
    sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.f, 0.f));
    sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.f, 0.25f));
    sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.f, 0.5f));

    sprite->setAnimationSpeed(MOVE_RIGHT, 8);
    sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25, 0.f));
    sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25, 0.25f));
    sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25, 0.5f));

    sprite->changeAnimation(MOVE_RIGHT);
    tileMapDispl = tileMapPos;
    sprite->setPosition(glm::vec2(float(tileMapDispl.x + position.x), float(tileMapDispl.y + position.y)));
}

void Vampire::update(int deltaTime)
{
    sprite->update(deltaTime);

    if (flying)
    {
        updateFlying(deltaTime);
    }
    else
    {
        updateWalking(deltaTime);
    }

    sprite->setPosition(glm::vec2(float(tileMapDispl.x + position.x), float(tileMapDispl.y + position.y)));
}

void Vampire::updateFlying(int deltaTime)
{
    const glm::ivec2 tilePosition = position / map->getTileSize();

    MovementRange &otherRange = currentMovementStage == 0 ? rangeStage2 : rangeStage1;
    if (tilePosition == otherRange.tileStart)
    {
        flying = false;

        if (otherRange.tileEnd.x > otherRange.tileStart.x)
            sprite->changeAnimation(MOVE_RIGHT);
        else
            sprite->changeAnimation(MOVE_LEFT);

        currentMovementStage = (currentMovementStage + 1) % 2;
    }
    else
    {
        auto movement = otherRange.tileStart - tilePosition;

        auto div = std::max(std::abs(movement.x), std::abs(movement.y));

        if (div != 0)
        {
            movement = movement / div;
            position += movement;
        }
    }
}

void Vampire::updateWalking(int deltaTime)
{
    position.x += movementSpeed;

    const glm::ivec2 tilePosition = position / map->getTileSize();
    MovementRange &currentRange = currentMovementStage == 0 ? rangeStage1 : rangeStage2;
    if (tilePosition == currentRange.tileEnd)
    {
        ++currentStageTimes;
        changeDirection();

        auto tmp = currentRange.tileEnd;
        currentRange.tileEnd = currentRange.tileStart;
        currentRange.tileStart = tmp;
    }

    if (currentStageTimes == STAGE_TIMES_FLY)
    {
        flying = true;
        currentStageTimes = 0;
    }

    // if (tilePosition == tileEnd)
    // {
    //     changeDirection();

    //     auto tmp = tileEnd;
    //     tileEnd = tileStart;
    //     tileStart = tmp;
    // }
}

void Vampire::changeDirection()
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

void Vampire::setDirection(CharacterAnims direction)
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

void Vampire::setMovementRange(const glm::ivec2 &tileStartOrigin, const glm::ivec2 &tileEndOrigin,
                               const glm::ivec2 &tileStartDest, const glm::ivec2 &tileEndDest)
{
    rangeStage1.tileStart = tileStartOrigin;
    rangeStage1.tileEnd = tileEndOrigin;

    rangeStage2.tileStart = tileStartDest;
    rangeStage2.tileEnd = tileEndDest;

    assert(tileStartOrigin.y == tileEndOrigin.y && tileStartDest.y == tileEndDest.y);

    if (rangeStage1.tileEnd.x > rangeStage1.tileStart.x)
        setDirection(MOVE_RIGHT);
    else
        setDirection(MOVE_LEFT);

    setPosition({rangeStage1.tileStart.x * map->getTileSize(), rangeStage1.tileStart.y * map->getTileSize()});
}