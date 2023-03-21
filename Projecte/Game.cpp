#include <GL/glew.h>
#include <GL/glut.h>
#include "Game.h"

static Scene *sceneLevel01()
{
	Scene *scene = new Scene();

	Scene::Description description{};

	description.levelName = "level01.txt";
	description.playerPositionStartTile = {8, 3};

	description.skeletonDescriptions = {
		Scene::EnemyDescription{22, 18, MOVE_RIGHT},
		Scene::EnemyDescription{8, 18, MOVE_LEFT}};

	description.vampireDescriptions = {
		Scene::EnemyDescription{6, 9, MOVE_RIGHT},
		Scene::EnemyDescription{24, 9, MOVE_LEFT}};

	description.keyPositionTile = {8, 18};
	description.doorPositionTile = {21, 4};

	scene->init(description);

	return scene;
}

Game::Game()
{
	Scene *level01 = sceneLevel01();
	Scene *level02 = sceneLevel01();

	levels.push_back(level01);
	levels.push_back(level02);

	currentLevelIdx = 0;
	currentScene = levels[currentLevelIdx];
}

void Game::init()
{
	bPlay = true;
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
}

bool Game::update(int deltaTime)
{
	auto status = currentScene->update(deltaTime);

	if (status == SceneStatus::LevelComplete)
	{
		++currentLevelIdx;
		currentScene = levels[currentLevelIdx];
	}
    else if (status == SceneStatus::PlayerDead)
    {
        ++currentLevelIdx;
        currentScene = levels[currentLevelIdx];
    }

	return bPlay;
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	currentScene->render();
}

void Game::keyPressed(int key)
{
	if (key == 27) // Escape code
		bPlay = false;
	keys[key] = true;
}

void Game::keyReleased(int key)
{
	keys[key] = false;
}

void Game::specialKeyPressed(int key)
{
	specialKeys[key] = true;
}

void Game::specialKeyReleased(int key)
{
	specialKeys[key] = false;
}

void Game::mouseMove(int x, int y)
{
}

void Game::mousePress(int button)
{
}

void Game::mouseRelease(int button)
{
}

bool Game::getKey(int key) const
{
	return keys[key];
}

bool Game::getSpecialKey(int key) const
{
	return specialKeys[key];
}
