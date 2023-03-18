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

    timeSinceLastFly_ms = 0;

    setDirection(MOVE_RIGHT);

    tileMapDispl = tileMapPos;
    sprite->setPosition(glm::vec2(float(tileMapDispl.x + position.x), float(tileMapDispl.y + position.y)));
}

void Vampire::update(int deltaTime)
{
    sprite->update(deltaTime);
    timeSinceLastFly_ms += deltaTime;

    if (landing)
    {
        updateLanding(deltaTime);
    }
    else if (flying)
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
    position += flyingMovement;

    const bool collisionRight = map->collisionMoveRight(position, glm::ivec2(32), true);
    const bool collisionLeft = map->collisionMoveLeft(position, glm::ivec2(32), true);

    const bool collisionHorizontal = collisionRight || collisionLeft;

    if (collisionHorizontal)
    {
        position.x -= flyingMovement.x;
        flyingMovement = {-flyingMovement.x, flyingMovement.y};
    }

    const bool collisionUp = map->collisionMoveUp({position.x, position.y - 1}, glm::ivec2(32));
    // Bastante feo, pero mejor que cambiar collisionMoveDown para que
    // opcionalmente no tenga en cuenta las plataformas.
    bool collisionDown = false;
    {
        int x0 = position.x / map->getTileSize();
        int x1 = (position.x + 32) / map->getTileSize();

        for (int x = x0; x < x1; ++x)
        {
            if (map->isTileWithCollision({x, position.y / map->getTileSize() + 2}))
            {
                collisionDown = true;
                break;
            }
        }
    }

    const bool collisionVertical = collisionUp || collisionDown;

    if (collisionVertical)
    {
        position.y -= flyingMovement.y;
        flyingMovement = {flyingMovement.x, -flyingMovement.y};
    }

    if (timeSinceLastFly_ms / 1000 >= TIME_PER_STAGE)
    {
        landing = true;
        flying = false;
        timeSinceLastFly_ms = 0;
    }
}

void Vampire::updateWalking(int deltaTime)
{
    position.x += movementSpeed;
    position.y += FALL_STEP;

    map->collisionMoveDown(position, glm::ivec2(32), &position.y);

    const int nextYTile = position.y / map->getTileSize() + 2;
    const int nextXTile = currentDirection == MOVE_RIGHT
                              ? (position.x + 32 / 2) / map->getTileSize() + 1  // MOVE_RIGHT
                              : (position.x + 32 / 2) / map->getTileSize() - 1; // MOVE_LEFT

    const glm::ivec2 nextTile = glm::ivec2(nextXTile, nextYTile);

    const bool nextTileWouldFall = !map->isTileWithCollision(nextTile) && !map->isPlatform(nextTile);
    const bool collisionRight = map->collisionMoveRight(position, glm::ivec2(32), false);
    const bool collisionLeft = map->collisionMoveLeft(position, glm::ivec2(32), false);

    if (nextTileWouldFall || collisionLeft || collisionRight)
    {
        position.x -= movementSpeed;
        changeDirection();
    }

    if (timeSinceLastFly_ms / 1000 >= TIME_PER_STAGE)
    {
        flying = true;
        flyingMovement = {movementSpeed, -VAMPIRE_MOVEMENT_SPEED};
        timeSinceLastFly_ms = 0;
    }
}

void Vampire::updateLanding(int deltaTime)
{
    if (map->collisionMoveDown(position, glm::ivec2(32), &position.y))
    {
        landing = false;
        assert(!flying);

        CharacterAnims newDirection = flyingMovement.x >= 0 ? MOVE_RIGHT : MOVE_LEFT;
        setDirection(newDirection);
    }

    timeSinceLastFly_ms = 0;
    if (landing)
    {
        updateFlying(deltaTime);
    }
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
