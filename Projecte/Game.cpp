#include <GL/glew.h>
#include <GL/glut.h>

#include "Game.h"
#include "ShaderSystem.h"
#include "SoundManager.h"

int Game::score = 0;
int Game::lives = MAX_LIVES;

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

	description.score = &Game::score;
	description.stageNumber = 1;

	description.lives = &Game::lives;

	return description;
}

static Scene::Description sceneLevel02()
{
	Scene::Description description{};

	description.levelName = "level02.txt";
	description.playerPositionStartTile = {4, 18};

	description.skeletonDescriptions = {
		Scene::EnemyDescription{16, 12, MOVE_LEFT},
	};

	description.vampireDescriptions = {
		Scene::EnemyDescription{12, 6, MOVE_LEFT},
		Scene::EnemyDescription{18, 6, MOVE_RIGHT},
	};

	description.ghostDescriptions = {
		Scene::EnemyDescription{4, 6, MOVE_RIGHT},
		Scene::EnemyDescription{26, 6, MOVE_LEFT},
	};

	description.keyPositionTile = {12, 18};
	description.doorPositionTile = {20, 4};

	description.score = &Game::score;
	description.stageNumber = 2;

	description.lives = &Game::lives;

	return description;
}

static Scene::Description sceneLevel03()
{
	Scene::Description description{};

	description.levelName = "level03.txt";
	description.playerPositionStartTile = {24, 18};

	description.skeletonDescriptions = {
		Scene::EnemyDescription{18, 6, MOVE_RIGHT},
		Scene::EnemyDescription{12, 6, MOVE_LEFT},
	};

	description.vampireDescriptions = {
		Scene::EnemyDescription{2, 15, MOVE_RIGHT},
		Scene::EnemyDescription{18, 3, MOVE_RIGHT},
	};

	description.ghostDescriptions = {
		Scene::EnemyDescription{15, 8, MOVE_RIGHT},
	};

	description.keyPositionTile = {24, 18};
	description.doorPositionTile = {7, 4};

	description.score = &Game::score;
	description.stageNumber = 3;

	description.lives = &Game::lives;

	return description;
}

Game::Game()
{
	levelDescriptions.push_back(sceneLevel01());
	levelDescriptions.push_back(sceneLevel02());
	levelDescriptions.push_back(sceneLevel03());

	currentLevelIdx = 0;
	currentSceneType = SceneType::MainMenu;

	ShaderSystem::init();
}

void Game::init()
{
	bPlay = true;
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	// levelSelection = new LevelSelection();
	mainMenu = new MainMenu(SCREEN_WIDTH, SCREEN_HEIGHT);
	instructionsMenu = new TexturedMenu(SCREEN_WIDTH, SCREEN_HEIGHT, "images/main_menu/instructions_menu.png");
	creditsMenu = new TexturedMenu(SCREEN_WIDTH, SCREEN_HEIGHT, "images/main_menu/credits_menu.png");
	playerDeadScreen = new PlayerDeadScreen(SCREEN_WIDTH, SCREEN_HEIGHT, ReasonLostGame::PlayerDead);
	gamePausedScreen = new GamePausedScreen(SCREEN_WIDTH, SCREEN_HEIGHT);
    timeIsUpScreen = new PlayerDeadScreen(SCREEN_WIDTH, SCREEN_HEIGHT, ReasonLostGame::TimeIsUp);
	gameFinishedScreen = new GameFinishedScreen(SCREEN_WIDTH, SCREEN_HEIGHT, &Game::score);
}

bool Game::update(int deltaTime)
{
    if (timeToWaitForMenuSoundtrack > 0)
    {
        timeToWaitForMenuSoundtrack -= deltaTime;
        if (timeToWaitForMenuSoundtrack <= 0) SoundManager::getManager().playSoundtrack("sounds/menus.wav");
    }

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
	else if (currentSceneType == SceneType::GameFinished)
	{
		const auto status = gameFinishedScreen->update(deltaTime);
		if (status == (int)GameFinishedSelection::Credits)
			currentSceneType = SceneType::Credits;
		else if (status == (int)GameFinishedSelection::MainMenu)
			currentSceneType = SceneType::MainMenu;

		if (status != (int)GameFinishedSelection::None)
		{
			SoundManager::getManager().playSoundtrack("sounds/menus.wav");
            timeToWaitForMenuSoundtrack = 0;
		}

		keys[13] = false;
	}
	else if (currentSceneType == SceneType::Play && !paused)
	{
		SceneStatus status;
		if (!playerDead and !timeIsUp)
			status = currentPlayScene->update(deltaTime);
		else if (playerDead)
			status = SceneStatus::PlayerDead;
        else
            status = SceneStatus::TimeIsUp;

		if (status == SceneStatus::LevelComplete)
		{
			if (currentLevelIdx == levelDescriptions.size() - 1)
			{
				currentSceneType = SceneType::GameFinished;
				SoundManager::getManager().playSoundtrack("sounds/game_complete.wav");
                timeToWaitForMenuSoundtrack = 30000;
			}
			else
			{
				nextLevel();
			}
		}
		else if (status == SceneStatus::PlayerDead or status == SceneStatus::TimeIsUp)
		{
			SoundManager::getManager().playSoundtrack("sounds/menus.wav");
			playerDead = status == SceneStatus::PlayerDead;
            timeIsUp = not playerDead;
			const auto player_dead_status = playerDead ?
                    static_cast<PlayerDeadSelection>(playerDeadScreen->update(deltaTime)) :
                    static_cast<PlayerDeadSelection>(timeIsUpScreen->update(deltaTime));

			if (player_dead_status == PlayerDeadSelection::StartAgain)
			{
				Game::score = 0;
				Game::lives = MAX_LIVES;
				playerDead = false;
                timeIsUp = false;
				currentSceneType = SceneType::Play;

				currentLevelIdx = 0;
				changeToLevel(0);
			}
			else if (player_dead_status == PlayerDeadSelection::ExitMainMenu)
			{
				playerDead = false;
                timeIsUp = false;
				currentSceneType = SceneType::MainMenu;
				keys[13] = false; // Set enter to false
			}
			else if (player_dead_status == PlayerDeadSelection::ExitDesktop)
			{
				bPlay = false;
			}
		}
	}
	else if (currentSceneType == SceneType::Play && paused)
	{
		const auto status = gamePausedScreen->update(deltaTime);
		if (status == (int)GamePausedSelection::Continue)
		{
			paused = false;
			SoundManager::getManager().playSoundtrack("sounds/gameplaySoundtrack.wav");
		}
		else if (status == (int)GamePausedSelection::Restart)
		{
			Game::score = 0;
			Game::lives = MAX_LIVES;
			playerDead = false;
            timeIsUp = false;
			paused = false;
			currentLevelIdx = 0;

			changeToLevel(currentLevelIdx);
		}
		else if (status == (int)GamePausedSelection::ExitMainMenu)
		{
			currentSceneType = SceneType::MainMenu;
			paused = false;
			keys[13] = false; // Set enter to false
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
	case SceneType::Credits:
		creditsMenu->render();
		break;
	case SceneType::GameFinished:
		gameFinishedScreen->render();
		break;
	case SceneType::Play:
	{
		currentPlayScene->render();
		if (playerDead)
			playerDeadScreen->render();
		if (paused)
			gamePausedScreen->render();
        if (timeIsUp)
            timeIsUpScreen->render();
		break;
	}
	}
}

void Game::keyPressed(int key)
{
	if (currentSceneType == SceneType::Play && key == 'p' && !playerDead && !timeIsUp)
	{
		paused = !paused;
		if (paused)
			SoundManager::getManager().playSoundtrack("sounds/menus.wav");
		else
			SoundManager::getManager().playSoundtrack("sounds/gameplaySoundtrack.wav");
	}

	if (currentSceneType == SceneType::Play && key >= '1' && key <= '9')
	{
		const int newLevel = key - '1';
		if (newLevel == currentLevelIdx)
			return;

		score = 0;
		lives = MAX_LIVES;
		currentLevelIdx = newLevel;
		changeToLevel(newLevel);
	}

	if (currentSceneType == SceneType::Play && key == 'r')
	{
		lives = MAX_LIVES;
		score = 0;
		changeToLevel(currentLevelIdx); // basically means restart
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
	score = 0;
	lives = MAX_LIVES;

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
	{
		stopPlay();
		return;
	}

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