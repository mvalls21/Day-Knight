#include "Vampire.h"

#include <GL/glew.h>
#include <GL/glut.h>
#include <cmath>
#include <iostream>

#include "Game.h"

constexpr glm::ivec2 BAT_SIZE = glm::ivec2(24, 16);

void Vampire::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram)
{
    vampireTexture = new Texture();
    vampireTexture->loadFromFile("images/vampire.png", TEXTURE_PIXEL_FORMAT_RGBA);

    vampireSprite = AnimatedSprite::createSprite(glm::ivec2(32), glm::vec2(1.0f / 4.0f, 1.0f), vampireTexture, &shaderProgram);
    vampireSprite->setNumberAnimations(4);

    vampireSprite->setAnimationSpeed(STAND_LEFT, 8);
    vampireSprite->addKeyframe(STAND_LEFT, glm::vec2(0.f, 0.f));

    vampireSprite->setAnimationSpeed(STAND_RIGHT, 8);
    vampireSprite->addKeyframe(STAND_RIGHT, glm::vec2(0.25f, 0.f));

    vampireSprite->setAnimationSpeed(MOVE_LEFT, 8);
    vampireSprite->addKeyframe(MOVE_LEFT, glm::vec2(2.0f / 4.0f, 0.f));
    vampireSprite->addKeyframe(MOVE_LEFT, glm::vec2(3.0f / 4.0f, 0.0f));

    vampireSprite->setAnimationSpeed(MOVE_RIGHT, 8);
    vampireSprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.0f / 4.0f, 0.f));
    vampireSprite->addKeyframe(MOVE_RIGHT, glm::vec2(1.0f / 4.0f, 0.0f));

    batTexture = new Texture();
    batTexture->loadFromFile("images/bat.png", TEXTURE_PIXEL_FORMAT_RGBA);

    batSprite = AnimatedSprite::createSprite(BAT_SIZE, glm::vec2(1.0f / 3.0f, 1.0f), batTexture, &shaderProgram);
    batSprite->setNumberAnimations(1);

    batSprite->setAnimationSpeed(0, 8);
    batSprite->addKeyframe(0, glm::vec2(0.0f / 3.0f, 0.0f));
    batSprite->addKeyframe(0, glm::vec2(1.0f / 3.0f, 0.0f));
    batSprite->addKeyframe(0, glm::vec2(2.0f / 3.0f, 0.0f));

    batSprite->changeAnimation(0);

    sprite = vampireSprite;
    timeSinceLastFly_ms = 0;

    setDirection(MOVE_RIGHT);

    tileMapDispl = tileMapPos;
    sprite->setPosition(glm::vec2(float(tileMapDispl.x + position.x), float(tileMapDispl.y + position.y)));
}

Vampire::~Vampire()
{
    delete batTexture;
    delete vampireTexture;

    delete batSprite;
    delete vampireSprite;

    sprite = nullptr;
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

    const bool collisionRight = map->collisionMoveRight(position, BAT_SIZE, true);
    const bool collisionLeft = map->collisionMoveLeft(position, BAT_SIZE, true);

    const bool collisionHorizontal = collisionRight || collisionLeft;

    if (collisionHorizontal)
    {
        position.x -= flyingMovement.x;
        flyingMovement = {-flyingMovement.x, flyingMovement.y};
    }

    bool collisionUp = false;
    if (flyingMovement.y < 0)
    {
        collisionUp = map->collisionMoveUp({position.x, position.y - 1}, BAT_SIZE, false);
    }

    bool collisionDown = false;
    if (flyingMovement.y > 0)
    {
        collisionDown = map->collisionMoveDown({position.x, position.y}, BAT_SIZE, &position.y, false);
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
    defaultMovement(deltaTime);

    if (timeSinceLastFly_ms / 1000 >= TIME_PER_STAGE)
    {
        sprite = batSprite;

        flying = true;
        flyingMovement = {movementSpeed, -DEFAULT_ENEMY_MOVEMENT_SPEED};
        timeSinceLastFly_ms = 0;
    }
}

void Vampire::updateLanding(int deltaTime)
{
    if (map->collisionMoveDown(position, glm::ivec2(BAT_SIZE.x, 32), &position.y, false))
    {
        landing = false;
        assert(!flying);

        sprite = vampireSprite;

        CharacterAnims newDirection = flyingMovement.x >= 0 ? MOVE_RIGHT : MOVE_LEFT;
        setDirection(newDirection);
    }

    timeSinceLastFly_ms = 0;
    if (landing)
    {
        updateFlying(deltaTime);
    }
}

BoundingBoxInfo Vampire::getBoundingBoxInfo() const
{
    if (flying || landing)
    {
        return {
            .xoffset = 3,
            .yoffset = 3,
            .width = 12,
            .height = 12};
    }
    else
    {
        return {
            .xoffset = 3,
            .yoffset = 3,
            .width = 24,
            .height = 24};
    }
}
