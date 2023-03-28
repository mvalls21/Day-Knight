#pragma once

#include "Character.h"
#include "Sprite.h"
#include "TileMap.h"

#define MAX_LIVES 3

// Player is basically a Sprite that represents the player. As such it has
// all properties it needs to track its movement, jumping, and collisions.

class Player : public Character
{
public:
	void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram) override;
	void update(int deltaTime) override;
    int getLives() const;
    void setLives(int lives);
    void makeImmune(int milliseconds);
    bool isImmune() const;
    void render() const override;


private:
	bool bJumping;
	int jumpAngle, startY;
    int lives = MAX_LIVES;
    int remainingImmunityMilliseconds = 0;
    ShaderProgram* texProgram;
};
