#pragma once

#include "Sprite.h"
#include "TileMap.h"
#include "Character.h"

#define SKELETON_MOVEMENT_SPEED 1

class Skeleton : public Character
{
public:
	void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram) override;
	void update(int deltaTime) override;

	void setMovementRange(const glm::ivec2 &tileStart, const glm::ivec2 &tileEnd);

private:
	void changeDirection();
	void setDirection(CharacterAnims direction);

	CharacterAnims currentDirection;
	glm::ivec2 tileStart;
	glm::ivec2 tileEnd;

	int movementSpeed = SKELETON_MOVEMENT_SPEED;
};