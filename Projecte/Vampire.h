#pragma once

#include "Sprite.h"
#include "TileMap.h"
#include "Character.h"

#define VAMPIRE_MOVEMENT_SPEED 1

#define STAGE_TIMES_FLY 4

struct MovementRange
{
    glm::ivec2 tileStart;
    glm::ivec2 tileEnd;
};

class Vampire : public Character
{
public:
    void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram) override;
    void update(int deltaTime) override;

    void updateFlying(int deltaTime);
    void updateWalking(int deltaTime);

    void setMovementRange(const glm::ivec2 &tileStartOrigin, const glm::ivec2 &tileEndOrigin,
                          const glm::ivec2 &tileStartDest, const glm::ivec2 &tileEndDest);

private:
    void changeDirection();
    void setDirection(CharacterAnims direction);

    CharacterAnims currentDirection;

    MovementRange rangeStage1;
    MovementRange rangeStage2;

    bool currentMovementStage = 0;
    int currentStageTimes = 0;

    bool flying;

    int movementSpeed = VAMPIRE_MOVEMENT_SPEED;
};