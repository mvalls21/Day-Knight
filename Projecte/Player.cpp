#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"

#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 72

void Player::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram)
{
	bJumping = false;
    texProgram = &shaderProgram;
	spritesheet.loadFromFile("images/main_player.png", TEXTURE_PIXEL_FORMAT_RGBA);

	sprite = AnimatedSprite::createSprite(glm::ivec2(32, 32), glm::vec2(1.0f/6.0f, 1.0f/5.0f), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(4);

	sprite->setAnimationSpeed(STAND_LEFT, 6);
	sprite->addKeyframe(STAND_LEFT, glm::vec2(0.0f / 6.0f,  3.0f/5.0f));
	sprite->addKeyframe(STAND_LEFT, glm::vec2(1.0f / 6.0f,  3.0f/5.0f));
	sprite->addKeyframe(STAND_LEFT, glm::vec2(2.0f / 6.0f,  3.0f/5.0f));

	sprite->setAnimationSpeed(STAND_RIGHT, 6);
	sprite->addKeyframe(STAND_RIGHT, glm::vec2(0.0f / 6.0f,  1.0f/5.0f));
	sprite->addKeyframe(STAND_RIGHT, glm::vec2(1.0f / 6.0f,  1.0f/5.0f));
	sprite->addKeyframe(STAND_RIGHT, glm::vec2(2.0f / 6.0f,  1.0f/5.0f));

	sprite->setAnimationSpeed(MOVE_LEFT, 8);
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.0f / 6.0f, 2.0f / 5.0f));
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(1.0f / 6.0f, 2.0f / 5.0f));
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(2.0f / 6.0f, 2.0f / 5.0f));
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(3.0f / 6.0f, 2.0f / 5.0f));
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(4.0f / 6.0f, 2.0f / 5.0f));
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(5.0f / 6.0f, 2.0f / 5.0f));

	sprite->setAnimationSpeed(MOVE_RIGHT, 8);
	sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.0f / 6.0f, 0.0f));
	sprite->addKeyframe(MOVE_RIGHT, glm::vec2(1.0f / 6.0f, 0.0f));
	sprite->addKeyframe(MOVE_RIGHT, glm::vec2(2.0f / 6.0f, 0.0f));
	sprite->addKeyframe(MOVE_RIGHT, glm::vec2(3.0f / 6.0f, 0.0f));
	sprite->addKeyframe(MOVE_RIGHT, glm::vec2(4.0f / 6.0f, 0.0f));
	sprite->addKeyframe(MOVE_RIGHT, glm::vec2(5.0f / 6.0f, 0.0f));

	sprite->changeAnimation(0);
	tileMapDispl = tileMapPos;
    sprite->setPosition(glm::vec2(float(tileMapDispl.x + position.x), float(tileMapDispl.y + position.y)));
}

void Player::update(int deltaTime)
{
    if (remainingImmunityMilliseconds > 0)
    {
        remainingImmunityMilliseconds -= deltaTime;
    }

	sprite->update(deltaTime);
	if (Game::instance().getSpecialKey(GLUT_KEY_LEFT))
	{
		if (sprite->animation() != MOVE_LEFT)
			sprite->changeAnimation(MOVE_LEFT);
		position.x -= 2;
		if (map->collisionMoveLeft(position, glm::ivec2(32, 32), bJumping))
		{
			position.x += 2;
			sprite->changeAnimation(STAND_LEFT);
		}
	}
	else if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT))
	{
		if (sprite->animation() != MOVE_RIGHT)
			sprite->changeAnimation(MOVE_RIGHT);
		position.x += 2;
		if (map->collisionMoveRight(position, glm::ivec2(32, 32), bJumping))
		{
			position.x -= 2;
			sprite->changeAnimation(STAND_RIGHT);
		}
	}
	else
	{
		if (sprite->animation() == MOVE_LEFT)
			sprite->changeAnimation(STAND_LEFT);
		else if (sprite->animation() == MOVE_RIGHT)
			sprite->changeAnimation(STAND_RIGHT);
	}

	if (bJumping)
	{
		jumpAngle += JUMP_ANGLE_STEP;
		if (jumpAngle == 180)
		{
			bJumping = false;
			position.y = startY;
		}
		else
		{
			position.y = int(startY - JUMP_HEIGHT * sin(3.14159f * jumpAngle / 180.f));
			if(jumpAngle > 90)
			{
				bJumping = !map->collisionMoveDown(position, glm::ivec2(24, 32), &position.y, true);
			}
			else if (map->collisionMoveUp(position, glm::ivec2(32, 32)))
			{
				jumpAngle = 180 - jumpAngle;
			}
		}
	}
	else
	{
		position.y += FALL_STEP;
        if (map->collisionSpikes(position, glm::ivec2(24, 32)) and not isImmune())
        {
            setLives(getLives() - 1);
            makeImmune(PLAYER_IMMUNITY_MS);
        }

		if (map->collisionMoveDown(position, glm::ivec2(24, 32), &position.y, true))
		{
			if (Game::instance().getSpecialKey(GLUT_KEY_UP))
			{
				bJumping = true;
				jumpAngle = 0;
				startY = position.y;
			}
		}
	}

	sprite->setPosition(glm::vec2(float(tileMapDispl.x + position.x), float(tileMapDispl.y + position.y)));
}

int Player::getLives() const
{
    return lives;
}

bool Player::isImmune()
{
    return remainingImmunityMilliseconds > 0;
}

void Player::setLives(int lives)
{
    if (lives >= this->lives or remainingImmunityMilliseconds <= 0)
    {
        this->lives = max(min(lives, MAX_LIVES), 0);
    }
}

void Player::makeImmune(int milliseconds)
{
    remainingImmunityMilliseconds = milliseconds;
}

void Player::render() const {
    float val = cos(remainingImmunityMilliseconds/50.f);
    texProgram->setUniform4f("color", val, val, val, 1.0f);
    Character::render();
    texProgram->setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
}