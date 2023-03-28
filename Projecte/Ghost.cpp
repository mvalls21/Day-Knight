#include "Ghost.h"

void Ghost::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram)
{
    spritesheet.loadFromFile("images/ghost.png", TEXTURE_PIXEL_FORMAT_RGBA);

    sprite = AnimatedSprite::createSprite(glm::ivec2(32), glm::vec2(1.0f / 4.0f, 1.0f / 2.0f), &spritesheet, &shaderProgram);
    sprite->setNumberAnimations(4);

    sprite->setAnimationSpeed(STAND_LEFT, 8);
    sprite->addKeyframe(STAND_LEFT, glm::vec2(0.f, 0.f));

    sprite->setAnimationSpeed(STAND_RIGHT, 8);
    sprite->addKeyframe(STAND_RIGHT, glm::vec2(0.25f, 0.f));

    sprite->setAnimationSpeed(MOVE_LEFT, 8);
    sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.0f / 4.0f, 0.5f));
    sprite->addKeyframe(MOVE_LEFT, glm::vec2(1.0f / 4.0f, 0.5f));
    sprite->addKeyframe(MOVE_LEFT, glm::vec2(2.0f / 4.0f, 0.5f));
    sprite->addKeyframe(MOVE_LEFT, glm::vec2(3.0f / 4.0f, 0.5f));

    sprite->setAnimationSpeed(MOVE_RIGHT, 8);
    sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.0f / 4.0f, 0.0f));
    sprite->addKeyframe(MOVE_RIGHT, glm::vec2(1.0f / 4.0f, 0.0f));
    sprite->addKeyframe(MOVE_RIGHT, glm::vec2(2.0f / 4.0f, 0.0f));
    sprite->addKeyframe(MOVE_RIGHT, glm::vec2(3.0f / 4.0f, 0.0f));

    setDirection(MOVE_RIGHT);

    tileMapDispl = tileMapPos;
    sprite->setPosition(glm::vec2(float(tileMapDispl.x + position.x), float(tileMapDispl.y + position.y)));
}

constexpr glm::vec2 GHOST_SIZE = glm::vec2(32.0f);

void Ghost::update(int deltaTime)
{
    sprite->update(deltaTime);

    position += flyingMovement;

    const bool collisionRight = map->collisionMoveRight(position, GHOST_SIZE, true, true);
    const bool collisionLeft = map->collisionMoveLeft(position, GHOST_SIZE, true, true);

    const bool collisionHorizontal = collisionRight || collisionLeft;

    if (collisionHorizontal)
    {
        position.x -= flyingMovement.x;
        flyingMovement = {-flyingMovement.x, flyingMovement.y};
        changeDirection();
    }

    bool collisionUp = false;
    if (flyingMovement.y < 0)
    {
        collisionUp = map->collisionMoveUp({position.x, position.y - 1}, GHOST_SIZE, false, true);
    }

    bool collisionDown = false;
    if (flyingMovement.y > 0)
    {
        collisionDown = map->collisionMoveDown({position.x, position.y}, GHOST_SIZE, &position.y, false, true);
    }

    const bool collisionVertical = collisionUp || collisionDown;

    if (collisionVertical)
    {
        position.y -= flyingMovement.y;
        flyingMovement = {flyingMovement.x, -flyingMovement.y};
    }

    sprite->setPosition(glm::vec2(float(tileMapDispl.x + position.x), float(tileMapDispl.y + position.y)));
}

BoundingBoxInfo Ghost::getBoundingBoxInfo() const
{
    return {
        .xoffset = 3,
        .yoffset = 3,
        .width = 20,
        .height = 20};
}