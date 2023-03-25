#pragma once

#include "Scene.h"

#include "LevelSelection.h"
#include "MainMenu.h"
#include "InstructionsMenu.h"

constexpr int SCREEN_WIDTH = 40 * (32 + 2);
constexpr int SCREEN_HEIGHT = 40 * (22 + 2) + 40;

// Game is a singleton (a class with a single instance) that represents our whole application

enum class SceneType
{
	MainMenu,
	Play,
	Instructions,
	Credits,
};

class Game
{
private:
	Game();

public:
	static Game &instance()
	{
		static Game G;

		return G;
	}

	void init();
	bool update(int deltaTime);
	void render();

	// Input callback methods
	void keyPressed(int key);
	void keyReleased(int key);
	void specialKeyPressed(int key);
	void specialKeyReleased(int key);
	void mouseMove(int x, int y);
	void mousePress(int button);
	void mouseRelease(int button);

	bool getKey(int key) const;
	bool getSpecialKey(int key) const;

private:
	bool bPlay;						  // Continue to play game?
	bool keys[256], specialKeys[256]; // Store key states so that we can have access at any time

	bool paused = false;

	SceneType currentSceneType;

	LevelSelection *levelSelection;
	MainMenu *mainMenu;
	InstructionsMenu* instructionsMenu;


	std::vector<Scene *> levels;
	int currentLevelIdx;
};
