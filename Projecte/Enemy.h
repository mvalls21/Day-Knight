#pragma once

#include "Character.h"
#include "ShaderProgram.h"

#define DEFAULT_ENEMY_MOVEMENT_SPEED 1

class Enemy : public Character
{
public:
    virtual void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram) = 0;
    virtual void update(int deltaTime) override;

    virtual void render() const override;

    void setDirection(CharacterAnims direction);

    void toggleDark() { isDark = !isDark; }
    void disableDark() { isDark = false; }

protected:
    CharacterAnims currentDirection;
    int movementSpeed = DEFAULT_ENEMY_MOVEMENT_SPEED;

    ShaderProgram *texProgram;
    bool isDark = false;

    void defaultMovement(int deltaTime);
    void changeDirection();
};