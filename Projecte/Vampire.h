#pragma once

#include "Sprite.h"
#include "TileMap.h"
#include "Character.h"

#define VAMPIRE_MOVEMENT_SPEED 1

#define TIME_PER_STAGE 10 // seconds

class Vampire : public Character
{
public:
    void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram) override;
    void update(int deltaTime) override;

    void setDirection(CharacterAnims direction);

private:
    void updateFlying(int deltaTime);
    void updateWalking(int deltaTime);
    void updateLanding(int deltaTime);

    void changeDirection();

    CharacterAnims currentDirection;
    int movementSpeed = VAMPIRE_MOVEMENT_SPEED;

    AnimatedSprite *vampireSprite;
    AnimatedSprite *batSprite;

    bool flying = false;
    bool landing = false;
    int timeSinceLastFly_ms;
    glm::ivec2 flyingMovement;
};