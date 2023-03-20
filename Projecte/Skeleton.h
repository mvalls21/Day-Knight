#pragma once

#include "Sprite.h"
#include "TileMap.h"

#include "Enemy.h"

class Skeleton : public Enemy
{
public:
	void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram) override;

private:
	int movementSpeed = DEFAULT_ENEMY_MOVEMENT_SPEED;
};