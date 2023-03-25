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
	currentSceneType = SceneType::MainMenu;
}

void Game::init()
{
	bPlay = true;
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	// levelSelection = new LevelSelection();
	mainMenu = new MainMenu(SCREEN_WIDTH, SCREEN_HEIGHT);
	instructionsMenu = new InstructionsMenu(SCREEN_WIDTH, SCREEN_HEIGHT);
}

bool Game::update(int deltaTime)
{
	if (currentSceneType == SceneType::MainMenu)
	{
		auto status = mainMenu->update(deltaTime);
		if (status == MainMenuSelection::Play)
			currentSceneType = SceneType::Play;
		if (status == MainMenuSelection::Instructions)
			currentSceneType = SceneType::Instructions;
		else if (status == MainMenuSelection::Exit)
			bPlay = false;
	}
	else if (currentSceneType == SceneType::Instructions)
	{
		auto status = instructionsMenu->update(deltaTime);
		if (status == (int)InstructionsMenuSelection::Back)
			currentSceneType = SceneType::MainMenu;
	}
	else if (currentSceneType == SceneType::Play && !paused)
	{
		auto status = levels[currentLevelIdx]->update(deltaTime);

		if (status == SceneStatus::LevelComplete)
			++currentLevelIdx;
		else if (status == SceneStatus::PlayerDead)
		{
			currentSceneType = SceneType::MainMenu;
			currentLevelIdx = 0;
		}
	}

	return bPlay;
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	switch (currentSceneType)
	{
	case SceneType::MainMenu:
		mainMenu->render();
		break;
	case SceneType::Instructions:
		instructionsMenu->render();
		break;
	case SceneType::Play:
		levels[currentLevelIdx]->render();
		break;
	}
}

void Game::keyPressed(int key)
{
	if (currentSceneType == SceneType::Play && key == 'p')
		paused = !paused;

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
