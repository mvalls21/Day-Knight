#include <GL/glew.h>
#include <GL/glut.h>
#include "Game.h"

static Scene::Description sceneLevel01()
{
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
	description.doorPositionTile = {19, 4};

	return description;
}

Game::Game()
{
	Scene::Description level01 = sceneLevel01();
	Scene::Description level02 = sceneLevel01();

	levelDescriptions.push_back(level01);
	levelDescriptions.push_back(level02);

	currentLevelIdx = 0;
	currentSceneType = SceneType::MainMenu;
}

void Game::init()
{
	bPlay = true;
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	// levelSelection = new LevelSelection();
	mainMenu = new MainMenu(SCREEN_WIDTH, SCREEN_HEIGHT);
	instructionsMenu = new TexturedMenu(SCREEN_WIDTH, SCREEN_HEIGHT, "images/main_menu/instructions_menu.png");
	creditsMenu = new TexturedMenu(SCREEN_WIDTH, SCREEN_HEIGHT, "images/main_menu/credits_menu.png");
}

bool Game::update(int deltaTime)
{
	if (currentSceneType == SceneType::MainMenu)
	{
		auto status = mainMenu->update(deltaTime);
		if (status == MainMenuSelection::Play)
			startPlay();
		if (status == MainMenuSelection::Instructions)
			currentSceneType = SceneType::Instructions;
		if (status == MainMenuSelection::Credits)
			currentSceneType = SceneType::Credits;
		else if (status == MainMenuSelection::Exit)
			bPlay = false;
	}
	else if (currentSceneType == SceneType::Instructions)
	{
		auto status = instructionsMenu->update(deltaTime);
		if (status == (int)TexturedMenuSelection::Back)
			currentSceneType = SceneType::MainMenu;
	}
	else if (currentSceneType == SceneType::Credits)
	{
		auto status = creditsMenu->update(deltaTime);
		if (status == (int)TexturedMenuSelection::Back)
			currentSceneType = SceneType::MainMenu;
	}
	else if (currentSceneType == SceneType::Play && !paused)
	{
		auto status = currentPlayScene->update(deltaTime);

		if (status == SceneStatus::LevelComplete)
			nextLevel();
		else if (status == SceneStatus::PlayerDead)
			stopPlay();
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
	case SceneType::Credits:
		creditsMenu->render();
		break;
	case SceneType::Play:
		currentPlayScene->render();
		break;
	}
}

void Game::keyPressed(int key)
{
	if (currentSceneType == SceneType::Play && key == 'p')
		paused = !paused;

	if (currentSceneType == SceneType::Play && key >= '1' && key <= '3')
	{
		changeToLevel(key - '1');
	}

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

void Game::startPlay()
{
	currentSceneType = SceneType::Play;
	currentLevelIdx = 0;

	if (currentPlayScene != nullptr)
		delete currentPlayScene;

	currentPlayScene = new Scene();
	currentPlayScene->init(levelDescriptions[currentLevelIdx]);
}

void Game::nextLevel()
{
	changeToLevel(++currentLevelIdx);
}

void Game::changeToLevel(int levelIdx)
{
	if (levelIdx < 0 || levelIdx >= levelDescriptions.size())
		return;

	if (currentPlayScene != nullptr)
		delete currentPlayScene;

	currentPlayScene = new Scene();
	currentPlayScene->init(levelDescriptions[levelIdx]);
}

void Game::stopPlay()
{
	delete currentPlayScene;
	currentPlayScene = nullptr;

	currentSceneType = SceneType::MainMenu;
}