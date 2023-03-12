#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"

#define SCREEN_X 16
#define SCREEN_Y 16

#define INIT_PLAYER_X_TILES 7
#define INIT_PLAYER_Y_TILES 17

#define KEY_POSITION_X_TILES 20
#define KEY_POSITION_Y_TILES 18

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
	map = TileMap::createTileMap("levels/level01.txt", glm::vec2(SCREEN_X, SCREEN_Y), mapProgram);
	player = new Player();
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));
	player->setTileMap(map);
	projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f);
	currentTime = 0.0f;

	Texture *tileset = new Texture();
	tileset->loadFromFile("images/castle-tileset.png", PixelFormat::TEXTURE_PIXEL_FORMAT_RGBA);

	keySprite = StaticSprite::createSprite(glm::vec2(16.0f), glm::vec2(1.0f / 8.0f, 1.0f / 16.0f), tileset, &texProgram);
	keySprite->setPosition({SCREEN_X + KEY_POSITION_X_TILES * 16.0f, SCREEN_Y + KEY_POSITION_Y_TILES * 16.0f});
	keySprite->setSpritesheetCoords(glm::vec2(2.0f / 8.0f, 11.0f / 16.0f));

	doorSprite = Sprite::createSprite(glm::vec2(32.0f, 32.0f), glm::vec2(1.0f / 4.0f, 1.0f / 8.0f), tileset, &texProgram);
	doorSprite->setPosition({SCREEN_X + 10 * 16, SCREEN_Y + 18.0 * 16.0});
	doorSprite->setNumberAnimations(2);
	doorSprite->addKeyframe(0, {2.0f / 8.0f, 9.0f / 16.0f});
	doorSprite->addKeyframe(1, {5.0f / 8.0f, 9.0f / 16.0f});
}

void Scene::update(int deltaTime)
{
	currentTime += deltaTime;
	player->update(deltaTime);

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
		}
	}
}

void Scene::render()
{
	glm::mat4 modelview;

	mapProgram.use();
	mapProgram.setUniformMatrix4f("projection", projection);
	mapProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
	modelview = glm::mat4(1.0f);
	mapProgram.setUniformMatrix4f("modelview", modelview);
	mapProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
	map->render();

	texProgram.use();
	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
	texProgram.setUniformMatrix4f("modelview", modelview);
	texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
	player->render();

	if (showKey)
		keySprite->render();

	if (!isDoorOpen)
		doorSprite->changeAnimation(0);
	else
		doorSprite->changeAnimation(1);

	doorSprite->render();
}

void Scene::initShaders()
{
	Shader vShader, fShader;

	// TEX PROGRAM
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

	// DOUBLE TEX PROGRAM - MAP WITH BACKGROUND
	vShader.initFromFile(VERTEX_SHADER, "shaders/map.vert");
	if (!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl
			 << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/map.frag");
	if (!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl
			 << endl;
	}
	mapProgram.init();
	mapProgram.addShader(vShader);
	mapProgram.addShader(fShader);
	mapProgram.link();
	if (!mapProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << mapProgram.log() << endl
			 << endl;
	}
	mapProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}
