#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "ShaderProgram.h"
#include "TileMap.h"

#include "Object.h"

#include "StaticSprite.h"
#include "AnimatedSprite.h"

#include "Character.h"
#include "Player.h"
#include "Skeleton.h"
#include "Vampire.h"

// Scene contains all the entities of our game.
// It is responsible for updating and render them.

class Scene
{

public:
	Scene();
	~Scene();

	void init();
	void update(int deltaTime);
	void render();

private:
	void initShaders();

private:
	TileMap *map;
	Player *player;

	Skeleton *skeleton;
	Vampire *vampire;

	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;

	bool showKey = false;
	Key* key;

	bool isDoorOpen = false;
	Door *door;
};
