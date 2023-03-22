#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "ShaderProgram.h"
#include "TileMap.h"

#include "Object.h"

#include "StaticSprite.h"
#include "AnimatedSprite.h"

#include "Player.h"
#include "Enemy.h"
#include "Skeleton.h"
#include "Vampire.h"
#include "Text.h"

// Scene contains all the entities of our game.
// It is responsible for updating and render them.

enum class SceneStatus
{
	Continue,
	PlayerDead,
	LevelComplete
};

class Scene
{
public:
	struct EnemyDescription
	{
		int tileX;
		int tileY;
		CharacterAnims startingDirection;
	};

	struct Description
	{
		std::string levelName;

		glm::ivec2 playerPositionStartTile;
		std::vector<EnemyDescription> skeletonDescriptions;
		std::vector<EnemyDescription> vampireDescriptions;
		// More enemy types...

		glm::ivec2 keyPositionTile;
		glm::ivec2 doorPositionTile;
	};

	Scene();
	~Scene();

	void init(const Description &description);
	SceneStatus update(int deltaTime);
	void render();

private:
	void initShaders();

private:
	TileMap *map;

	Player *player;
	std::vector<Enemy *> enemies;

	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;

	bool showKey = false;
	Key *key;

	bool isDoorOpen = false;
	Door *door;

    StaticSprite *heart;

	Gem* gem = nullptr;
	Clock* clock = nullptr;
};
