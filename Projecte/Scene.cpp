#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include "Scene.h"
#include "Game.h"

#include <random>

#define SCREEN_X 26
#define SCREEN_Y 46

#define INIT_PLAYER_X_TILES 8
#define INIT_PLAYER_Y_TILES 3

#define TILE_POS(x, y) glm::vec2(SCREEN_X + (x)*map->getTileSize(), SCREEN_Y + (y)*map->getTileSize())

Scene::Scene()
{
	map = NULL;
	player = NULL;
}

Scene::~Scene()
{
	delete map;
	delete tileset;

	delete player;
	for (const auto *enemy : enemies)
		delete enemy;

	delete key;
	delete door;
	delete heart;

	if (currentObject != nullptr)
		delete currentObject;
}

void Scene::init(const Description &description)
{
	initShaders();

	projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f);
	projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1) / 2.4f, float(SCREEN_HEIGHT - 1) / 2.4f, 0.f);
	currentTime = 0.0f;

	// 	Load level
	std::string levelPath = "levels/" + description.levelName;
	map = TileMap::createTileMap(levelPath, {SCREEN_X, SCREEN_Y}, texProgram);

	// Create player
	player = new Player();
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	player->setPosition(glm::vec2(description.playerPositionStartTile.x * map->getTileSize(), description.playerPositionStartTile.y * map->getTileSize()));
	player->setTileMap(map);

	// Create skeletons
	for (const auto &desc : description.skeletonDescriptions)
	{
		auto *skeleton = new Skeleton();
		skeleton->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
		skeleton->setTileMap(map);
		skeleton->setPosition({desc.tileX * map->getTileSize(), desc.tileY * map->getTileSize()});
		skeleton->setDirection(desc.startingDirection);

		enemies.push_back(skeleton);
	}

	// Create vampires
	for (const auto &desc : description.vampireDescriptions)
	{
		auto *vampire = new Vampire();
		vampire->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
		vampire->setTileMap(map);
		vampire->setPosition({desc.tileX * map->getTileSize(), desc.tileY * map->getTileSize()});
		vampire->setDirection(desc.startingDirection);

		enemies.push_back(vampire);
	}

	// Create ghosts
	for (const auto &desc : description.ghostDescriptions)
	{
		auto *ghost = new Ghost();
		ghost->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
		ghost->setTileMap(map);
		ghost->setPosition({desc.tileX * map->getTileSize(), desc.tileY * map->getTileSize()});
		ghost->setDirection(desc.startingDirection);

		enemies.push_back(ghost);
	}

	tileset = new Texture();
	tileset->loadFromFile("images/nuevo_tileset.png", PixelFormat::TEXTURE_PIXEL_FORMAT_RGBA);

	heart = StaticSprite::createSprite(glm::vec2(20.f), glm::vec2(1.f / 10.f), tileset, &texProgram);
	heart->setSpritesheetCoords(glm::vec2(5.f / 10.f, 4.f / 10.f));

	key = new Key(tileset, {SCREEN_X + description.keyPositionTile.x * map->getTileSize(), SCREEN_Y + description.keyPositionTile.y * map->getTileSize()}, &texProgram);

	const auto tileSize = map->getTileSize();
	const glm::ivec2 doorPositionTop = TILE_POS(description.doorPositionTile.x, description.doorPositionTile.y - 1);
	const glm::ivec2 doorPositionBottom = TILE_POS(description.doorPositionTile.x, description.doorPositionTile.y);
	door = new Door(tileset, doorPositionTop, doorPositionBottom, &texProgram);

	// Create objects with random order
	std::random_device r;
	std::default_random_engine engine(r());

	std::vector<ObjectType> objects = {ObjectType::Gem, ObjectType::Clock};
	std::shuffle(objects.begin(), objects.end(), engine);

	for (const auto &object : objects)
		remainingObjectTypes.push(object);
}

SceneStatus Scene::update(int deltaTime)
{
	currentTime += deltaTime;
	objectTimer += deltaTime;

	map->update(deltaTime);

	player->update(deltaTime);

	for (auto *enemy : enemies)
		enemy->update(deltaTime);

	bool enemyCollision = not player->isImmune() and std::any_of(enemies.begin(), enemies.end(), [&](const Enemy *enemy)
																 { return player->isColliding(*enemy); });

	if (enemyCollision)
	{
		player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));
		player->setLives(player->getLives() - 1);
		player->makeImmune(PLAYER_IMMUNITY_MS);
	}

	if (!showKey && !isDoorOpen && map->isCompleted())
		showKey = true;

	if (showKey && !isDoorOpen)
	{
		if (player->isColliding(*key))
		{
			isDoorOpen = true;
			showKey = false;
			door->open();
		}
	}

	if (player->getLives() == 0)
		return SceneStatus::PlayerDead;

	if (isDoorOpen && player->isColliding(*door))
		return SceneStatus::LevelComplete;

	// Object spawn logic
	if (currentObjectType != ObjectType::None)
		currentObject->update(deltaTime);

	if (objectTimer >= OBJECT_PERIOD)
	{
		// ObjectType::None means that player should not interact with object, but existance or not is controlled
		// by currentObject == nullptr or not
		objectTimer = 0;

		if (currentObject == nullptr)
		{
			spawnRandomObject();
		}
		else
		{
			delete currentObject;
			currentObject = nullptr;
			currentObjectType = ObjectType::None;
		}
	}

	return SceneStatus::Continue;
}

void Scene::render()
{
	glm::mat4 modelview;

	texProgram.use();

	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
	modelview = glm::mat4(1.0f);
	texProgram.setUniformMatrix4f("modelview", modelview);
	texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);

	map->render();
	door->render();

	for (int i = 0; i < player->getLives(); ++i)
	{
		heart->setPosition(glm::vec2(SCREEN_X + 20 * i, SCREEN_Y - 30));
		heart->render();
	}

	for (const auto *enemy : enemies)
		enemy->render();

	if (showKey)
		key->render();

	if (currentObjectType != ObjectType::None)
		currentObject->render();

	player->render();
}

void Scene::initShaders()
{
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/texture.vert");
	if (!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl
			 << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/texture.frag");
	if (!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl
			 << endl;
	}
	texProgram.init();
	texProgram.addShader(vShader);
	texProgram.addShader(fShader);
	texProgram.link();
	if (!texProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << texProgram.log() << endl
			 << endl;
	}
	texProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}

void Scene::spawnRandomObject()
{
	if (remainingObjectTypes.size() == 0)
		return;

	std::random_device r;
	std::default_random_engine engine(r());

	constexpr glm::vec2 offset = {0, 7};

	// Select platform
	const auto platforms = map->getPlatforms();
	std::uniform_int_distribution<int> platform_random(0, platforms.size() - 1);
	const int platformIdx = platform_random(engine);

	const auto platform = platforms[platformIdx];
	const auto pos = TILE_POS(platform.x, platform.y - 2) + offset;

	// Get Object
	currentObjectType = remainingObjectTypes.front();
	remainingObjectTypes.pop();
	switch (currentObjectType)
	{
	case ObjectType::Gem:
		currentObject = new Gem(tileset, pos, &texProgram);
		break;
	case ObjectType::Clock:
		currentObject = new Clock(tileset, pos, &texProgram);
		break;
	}
}