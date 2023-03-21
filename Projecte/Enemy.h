#pragma once

#include "Character.h"

#define DEFAULT_ENEMY_MOVEMENT_SPEED 1

class Enemy : public Character
{
public:
    virtual ~Enemy() {}

    virtual void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram) = 0;
    virtual void update(int deltaTime) override;

    void setDirection(CharacterAnims direction);

protected:
    CharacterAnims currentDirection;
    int movementSpeed = DEFAULT_ENEMY_MOVEMENT_SPEED;

    void defaultMovement(int deltaTime);
    void changeDirection();
};