#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include "Scene.h"
#include "Game.h"

#define SCREEN_X 16
#define SCREEN_Y 16

#define INIT_PLAYER_X_TILES 8
#define INIT_PLAYER_Y_TILES 3

#define KEY_POSITION_X_TILES 8
#define KEY_POSITION_Y_TILES 18

#define DOOR_POSITION_X_TILES 21
#define DOOR_POSITION_Y_TILES 4

Scene::Scene()
{
	map = NULL;
	player = NULL;
}

Scene::~Scene()
{
	if (map != NULL)
		delete map;
	if (player != NULL)
		delete player;
}

void Scene::init()
{
	initShaders();

	map = TileMap::createTileMap("levels/level01.txt", {SCREEN_X, SCREEN_Y}, texProgram);

	player = new Player();
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));
	player->setTileMap(map);

	skeleton = new Skeleton();
	skeleton->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	skeleton->setTileMap(map);
	skeleton->setMovementRange({19, 18}, {28, 18});

	vampire = new Vampire();
	vampire->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	vampire->setTileMap(map);
	vampire->setMovementRange({4, 9}, {12, 9}, {7, 2}, {22, 2});

	projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f);
	projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1) / 2.5f, float(SCREEN_HEIGHT - 1) / 2.5f, 0.f);
	currentTime = 0.0f;

	Texture *tileset = new Texture();
	tileset->loadFromFile("images/nuevo_tileset.png", PixelFormat::TEXTURE_PIXEL_FORMAT_RGBA);

	keySprite = StaticSprite::createSprite(glm::vec2(20.0f), glm::vec2(1.0f / 10.0f, 1.0f / 10.0f), tileset, &texProgram);
	keySprite->setPosition({SCREEN_X + KEY_POSITION_X_TILES * 16.0f, SCREEN_Y + KEY_POSITION_Y_TILES * 16.0f});
	keySprite->setSpritesheetCoords(glm::vec2(0.0f, 4.0f / 10.0f));

	// Door sprites
	{
		auto doorSprite1 = AnimatedSprite::createSprite(glm::vec2(16.0f), glm::vec2(1.0f / 10.0f, 1.0f / 10.0f), tileset, &texProgram);
		doorSprite1->setPosition({SCREEN_X + DOOR_POSITION_X_TILES * 16, SCREEN_Y + DOOR_POSITION_Y_TILES * 16.0});

		doorSprite1->setNumberAnimations(2);
		doorSprite1->addKeyframe(0, {0.0f / 10.0f, 3.0f / 10.0f});
		doorSprite1->addKeyframe(1, {2.0f / 10.0f, 3.0f / 10.0f});

		doorSprite1->changeAnimation(0);

		auto doorSprite2 = AnimatedSprite::createSprite(glm::vec2(16.0f), glm::vec2(1.0f / 10.0f, 1.0f / 10.0f), tileset, &texProgram);
		doorSprite2->setPosition({SCREEN_X + DOOR_POSITION_X_TILES * 16, SCREEN_Y + (DOOR_POSITION_Y_TILES - 1) * 16.0});

		doorSprite2->setNumberAnimations(2);
		doorSprite2->addKeyframe(0, {1.0f / 10.0f, 3.0f / 10.0f});
		doorSprite2->addKeyframe(1, {3.0f / 10.0f, 3.0f / 10.0f});

		doorSprite2->changeAnimation(0);

		doorSprites.push_back(doorSprite1);
		doorSprites.push_back(doorSprite2);
	}
}

void Scene::update(int deltaTime)
{
	currentTime += deltaTime;

	map->update(deltaTime);

	player->update(deltaTime);
	skeleton->update(deltaTime);
	vampire->update(deltaTime);

	if (player->isColliding(*skeleton))
	{
		player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));
	}

	if (player->isColliding(*vampire))
	{
		player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));
	}

	if (!showKey && !isDoorOpen && map->isCompleted())
	{
		showKey = true;
	}

	const auto &posPlayer = player->getPosition();
	const glm::ivec2 tilePosition = {
		posPlayer.x / map->getTileSize(),
		posPlayer.y / map->getTileSize(),
	};

	if (showKey && !isDoorOpen)
	{
		if ((tilePosition.x == KEY_POSITION_X_TILES || tilePosition.x + 1 == KEY_POSITION_X_TILES) && (tilePosition.y == KEY_POSITION_Y_TILES || tilePosition.y == KEY_POSITION_Y_TILES - 1))
		{
			isDoorOpen = true;
			showKey = false;
			std::for_each(doorSprites.begin(), doorSprites.end(), [](const auto &sprite)
						  { sprite->changeAnimation(1); });
		}
	}
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
	skeleton->render();
	vampire->render();

	if (showKey)
		keySprite->render();

	std::for_each(doorSprites.begin(), doorSprites.end(), [](const auto &sprite)
				  { sprite->render(); });

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
