#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <queue>

#include "ShaderProgram.h"
#include "TileMap.h"

#include "Object.h"

#include "StaticSprite.h"
#include "AnimatedSprite.h"

#include "Player.h"
#include "Enemy.h"
#include "Skeleton.h"
#include "Vampire.h"
#include "Ghost.h"

#include "Text.h"

// Scene contains all the entities of our game.
// It is responsible for updating and render them.

#define PLAYER_IMMUNITY_MS 1500

constexpr int OBJECT_PERIOD = 10'000;

constexpr int TIME_TO_START = 2'000;

constexpr int MAX_TIME_FOR_LEVEL = 60'000;

constexpr int TIME_PRIZE_CLOCK_OBJ = 20'000;

constexpr int GEM_SCORE_BONUS = 500;

enum class SceneStatus
{
    Continue,
	PlayerDead,
	LevelComplete
};

enum class ObjectType
{
	None,
	Clock,
	Gem,
	Shield,
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
		std::vector<EnemyDescription> ghostDescriptions;

		glm::ivec2 keyPositionTile;
		glm::ivec2 doorPositionTile;

        int stageNumber;
        int* score;
	};

	Scene();
	~Scene();

	void init(const Description &description);
	SceneStatus update(int deltaTime);
	void render();

private:
	TileMap *map;
	Texture *tileset;
    Texture *topBarTex;

	Player *player;
	std::vector<Enemy *> enemies;

	ShaderProgram* texProgram;
	float currentTime;
	glm::mat4 projection;

	bool showKey = false;
	Key *key;

	bool isDoorOpen = false;
	Door *door;

	StaticSprite *heart;
    StaticSprite *topBar;

	Object *currentObject = nullptr;
	ObjectType currentObjectType = ObjectType::None;
	int objectTimer = 0;

    int levelTimer = MAX_TIME_FOR_LEVEL;
    int* score;

    int stageNum;

	std::queue<ObjectType> remainingObjectTypes;

	bool shieldProtection = false;
	Shield *shield;

	void spawnRandomObject();

	// Cheat keys
	bool invulnerability = false;
	void detectCheatKeys();

	Text* text;
};
