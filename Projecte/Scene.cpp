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
	vampire->setPosition({4 * map->getTileSize(), 9 * map->getTileSize()});
	vampire->setDirection(MOVE_RIGHT);

	projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f);
	projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1) / 2.5f, float(SCREEN_HEIGHT - 1) / 2.5f, 0.f);
	currentTime = 0.0f;

	Texture *tileset = new Texture();
	tileset->loadFromFile("images/nuevo_tileset.png", PixelFormat::TEXTURE_PIXEL_FORMAT_RGBA);

	key = new Key(tileset, {SCREEN_X + KEY_POSITION_X_TILES * 16.0f, SCREEN_Y + KEY_POSITION_Y_TILES * 16.0f}, &texProgram);

	const glm::ivec2 doorPositionTop = {SCREEN_X + DOOR_POSITION_X_TILES * 16, SCREEN_Y + (DOOR_POSITION_Y_TILES - 1) * 16.0};
	const glm::ivec2 doorPositionBottom = {SCREEN_X + DOOR_POSITION_X_TILES * 16, SCREEN_Y + DOOR_POSITION_Y_TILES * 16.0};
	door = new Door(tileset, doorPositionTop, doorPositionBottom, &texProgram);
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
		if (player->isColliding(*key))
		{
			isDoorOpen = true;
			showKey = false;
			door->open();
		}
	}

	if (isDoorOpen && player->isColliding(*door))
	{
		std::cout << "NEXT LEEEEEEEVELLLL\n";
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
		key->render();

	door->render();

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
