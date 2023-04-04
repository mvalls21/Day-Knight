#ifndef _TILE_MAP_INCLUDE
#define _TILE_MAP_INCLUDE

#include <map>

#include <glm/glm.hpp>
#include "Texture.h"
#include "ShaderProgram.h"
#include "Sprite.h"

#include "StaticSprite.h"
#include "AnimatedSprite.h"

// Class Tilemap is capable of loading a tile map from a text file in a very
// simple format (see level01.txt for an example). With this information
// it builds a single VBO that contains all tiles. As a result the render
// method draws the whole map independently of what is visible.

constexpr int TILE_CHANGED_SCORE_BONUS = 10;

class TileMap
{

private:
	TileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program, int* score);

public:
	// Tile maps can only be created inside an OpenGL context
	static TileMap *createTileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program, int* score);

	~TileMap();

	void render() const;
	void update(int deltaTime);
	void free();

	int getTileSize() const { return tileSize; }

	bool collisionMoveLeft(const glm::ivec2 &pos, const glm::ivec2 &size, const bool &bJumping, const bool ghost = false) const;
	bool collisionMoveRight(const glm::ivec2 &pos, const glm::ivec2 &size, const bool &bJumping, const bool ghost = false) const;
	bool collisionMoveUp(const glm::ivec2 &pos, const glm::ivec2 &size, const bool bJumping, const bool ghost = false) const;
	bool collisionMoveDown(const glm::ivec2 &pos, const glm::ivec2 &size, float *posY, bool tileChanger, const bool ghost = false);

	bool collisionSpikes(const glm::ivec2 &pos, const glm::ivec2 &size);

	bool isTileWithCollision(const glm::ivec2 &tile);
	bool isPlatform(const glm::ivec2 &tile);

	std::vector<glm::ivec2> getPlatforms() const;

	bool isCompleted() const;

private:
	bool loadLevel(const string &levelFile);
	void prepareArrays(const glm::vec2 &minCoords, ShaderProgram &program);

	StaticSprite *getActiveChangeableTile(const int tile) const;
	void checkCollisionChangeableTile(int tileX, int tileY);

private:
	GLuint vao;
	GLuint vbo;
	ShaderProgram *shaderProgram;
	GLint posLocation, texCoordLocation;
	int nTiles;
	glm::ivec2 position, mapSize, tilesheetSize;
	int tileSize, blockSize;
	Texture tilesheet;
	glm::vec2 tileTexSize;
	int *map;

	StaticSprite *background;

	glm::vec2 offset;
	std::map<std::pair<int, int>, bool> changeableTiles;
	std::vector<StaticSprite *> changeableToActive;

	AnimatedSprite *torchSprite;
	std::vector<std::pair<int, int>> torchPositions;

    int* score;
};

#endif // _TILE_MAP_INCLUDE
