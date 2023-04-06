#include "Enemy.h"

void Enemy::update(int deltaTime)
{
    sprite->update(deltaTime);
    defaultMovement(deltaTime);

    sprite->setPosition(glm::vec2(float(tileMapDispl.x + position.x), float(tileMapDispl.y + position.y)));
}

void Enemy::render() const
{
    texProgram->use();

    if (isDark)
	    texProgram->setUniform4f("color", 0.2, 0.2, 0.2, 1.0f);

	Character::render();

	texProgram->setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
}

void Enemy::defaultMovement(int deltaTime)
{
    position.x += movementSpeed;
    position.y += FALL_STEP;

    map->collisionMoveDown(position, glm::ivec2(32), &position.y, false);

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
}

void Enemy::changeDirection()
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

void Enemy::setDirection(CharacterAnims direction)
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
        movementSpeed = DEFAULT_ENEMY_MOVEMENT_SPEED;
    else
        movementSpeed = -DEFAULT_ENEMY_MOVEMENT_SPEED;
}