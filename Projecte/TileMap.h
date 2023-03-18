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

class TileMap
{

private:
	TileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program);

public:
	// Tile maps can only be created inside an OpenGL context
	static TileMap *createTileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program);

	~TileMap();

	void render() const;
	void update(int deltaTime);
	void free();

	int getTileSize() const { return tileSize; }

	bool collisionMoveLeft(const glm::ivec2 &pos, const glm::ivec2 &size, const bool &bJumping) const;
	bool collisionMoveRight(const glm::ivec2 &pos, const glm::ivec2 &size, const bool &bJumping) const;
	bool collisionMoveUp(const glm::ivec2 &pos, const glm::ivec2 &size) const;
	bool collisionMoveDown(const glm::ivec2 &pos, const glm::ivec2 &size, int *posY, bool tileChanger = false);

	bool isTileWithCollision(const glm::ivec2& nextTile);

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
};

#endif // _TILE_MAP_INCLUDE
