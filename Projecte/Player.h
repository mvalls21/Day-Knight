#pragma once

#include "Character.h"
#include "Sprite.h"
#include "TileMap.h"

// Player is basically a Sprite that represents the player. As such it has
// all properties it needs to track its movement, jumping, and collisions.

class Player : public Character
{
public:
	void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram) override;
	void update(int deltaTime) override;

private:
	bool bJumping;
	int jumpAngle, startY;
};
