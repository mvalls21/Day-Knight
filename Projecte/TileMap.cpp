#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "TileMap.h"

#include <glm/gtc/matrix_transform.hpp>

// ================
// Helper Functions
// ================

// REMEMBER: Plus 1 because tiles in map are saved as +1
constexpr int SPIKES = 41 + 1;

constexpr int CHANGEABLE_PLATFORM_RANGE_START = 20 + 1;
constexpr int CHANGEABLE_PLATFORM_RANGE_END = 22 + 1;

inline bool isChangeableTile(int x)
{
	return x >= CHANGEABLE_PLATFORM_RANGE_START && x <= CHANGEABLE_PLATFORM_RANGE_END;
}

constexpr int WALL = 0 + 1;

inline bool isCollisionTile(int x)
{
	return x == WALL || (x >= (23 + 1) && x <= (26 + 1)) || (x >= (34 + 1) && x <= (37 + 1));
}

constexpr int TORCH = 42 + 1;


// ================

TileMap *TileMap::createTileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program)
{
	TileMap *map = new TileMap(levelFile, minCoords, program);

	return map;
}

TileMap::TileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program)
{
	loadLevel(levelFile);
	prepareArrays(minCoords, program);

	shaderProgram = &program;
	offset = minCoords;

	Texture *back = new Texture();
	back->loadFromFile("images/background.png", PixelFormat::TEXTURE_PIXEL_FORMAT_RGBA);

	background = StaticSprite::createSprite(16.0f * glm::vec2(32.0f, 22.0f), glm::vec2(1.0f), back, &program);
	background->setPosition({offset.x, offset.y});
	background->setSpritesheetCoords(glm::vec2(0.0f));

	// Changeable tiles
	auto *changeable1 = StaticSprite::createSprite(glm::vec2(tileSize), glm::vec2(1.0f / 10.0f, 1.0f / 10.0f), &tilesheet, &program);
	changeable1->setSpritesheetCoords(glm::vec2(7.0f / 10.0f, 2.0f / 10.0f));

	auto *changeable2 = StaticSprite::createSprite(glm::vec2(tileSize), glm::vec2(1.0f / 10.0f, 1.0f / 10.0f), &tilesheet, &program);
	changeable2->setSpritesheetCoords(glm::vec2(8.0f / 10.0f, 2.0f / 10.0f));

	auto *changeable3 = StaticSprite::createSprite(glm::vec2(tileSize), glm::vec2(1.0f / 10.0f, 1.0f / 10.0f), &tilesheet, &program);
	changeable3->setSpritesheetCoords(glm::vec2(9.0f / 10.0f, 2.0f / 10.0f));

	changeableToActive.push_back(changeable1);
	changeableToActive.push_back(changeable2);
	changeableToActive.push_back(changeable3);

	// Torches
	torchSprite = AnimatedSprite::createSprite(glm::vec2(tileSize), glm::vec2(1.0f / 10.0f, 1.0f / 10.0f), &tilesheet, &program);
	torchSprite->setNumberAnimations(1);

	torchSprite->setAnimationSpeed(0, 8);
	torchSprite->addKeyframe(0, glm::vec2(2.0f / 10.0f, 4.0f / 10.0f));
	torchSprite->addKeyframe(0, glm::vec2(3.0f / 10.0f, 4.0f / 10.0f));
	torchSprite->addKeyframe(0, glm::vec2(4.0f / 10.0f, 4.0f / 10.0f));

	torchSprite->changeAnimation(0);
}

TileMap::~TileMap()
{
	if (map != NULL)
		delete map;

	for (const auto *c : changeableToActive)
	{
		delete c;
	}
}

void TileMap::render() const
{
	background->render();

	glm::mat4 modelview = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.f));
	shaderProgram->setUniformMatrix4f("modelview", modelview);

	glEnable(GL_TEXTURE_2D);
	tilesheet.use();
	glBindVertexArray(vao);
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texCoordLocation);
	glDrawArrays(GL_TRIANGLES, 0, 6 * nTiles);

	// Draw stepped on tiles
	for (const auto &[position, isChanged] : changeableTiles)
	{
		if (isChanged)
		{
			const auto &[y, x] = position;

			const int tile = map[y * mapSize.x + x];
			StaticSprite *active = getActiveChangeableTile(tile);

			active->setPosition({offset.x + x * tileSize, offset.y + y * tileSize});
			active->render();
		}
	}

	// Draw torches
	for (const auto &[y, x] : torchPositions)
	{
		torchSprite->setPosition({offset.x + x * tileSize, offset.y + y * tileSize});
		torchSprite->render();
	}

	glDisable(GL_TEXTURE_2D);
}

void TileMap::update(int deltaTime)
{
	torchSprite->update(deltaTime);
}

void TileMap::free()
{
	glDeleteBuffers(1, &vbo);
}

bool TileMap::loadLevel(const string &levelFile)
{
	ifstream fin;
	string line, tilesheetFile;
	stringstream sstream;
	char tile;

	fin.open(levelFile.c_str());
	if (!fin.is_open())
		return false;
	getline(fin, line);
	if (line.compare(0, 7, "TILEMAP") != 0)
		return false;
	getline(fin, line);
	sstream.str(line);
	sstream >> mapSize.x >> mapSize.y;
	getline(fin, line);
	sstream.str(line);
	sstream >> tileSize >> blockSize;
	getline(fin, line);
	sstream.str(line);
	sstream >> tilesheetFile;
	tilesheet.loadFromFile(tilesheetFile, TEXTURE_PIXEL_FORMAT_RGBA);
	tilesheet.setWrapS(GL_CLAMP_TO_EDGE);
	tilesheet.setWrapT(GL_CLAMP_TO_EDGE);
	tilesheet.setMinFilter(GL_NEAREST);
	tilesheet.setMagFilter(GL_NEAREST);
	getline(fin, line);
	sstream.str(line);
	sstream >> tilesheetSize.x >> tilesheetSize.y;
	tileTexSize = glm::vec2(1.f / tilesheetSize.x, 1.f / tilesheetSize.y);

	map = new int[mapSize.x * mapSize.y];
	for (int j = 0; j < mapSize.y; j++)
	{
		for (int i = 0; i < mapSize.x; i++)
		{
			int value;
			fin >> value;

			if (isChangeableTile(value + 1))
			{
				changeableTiles.insert({{j, i}, false});
			}

			if (value + 1 == TORCH)
			{
				torchPositions.push_back({j, i});
			}

			if (value == -1)
			{
				map[j * mapSize.x + i] = 0;
			}
			else
			{
				map[j * mapSize.x + i] = value + 1;
			}
		}
		fin.get(tile);
#ifndef _WIN32
		fin.get(tile);
#endif
	}
	fin.close();

	return true;
}

void TileMap::prepareArrays(const glm::vec2 &minCoords, ShaderProgram &program)
{
	int tile;
	glm::vec2 posTile, texCoordTile[2], halfTexel;
	vector<float> vertices;

	nTiles = 0;
	halfTexel = glm::vec2(0.5f / tilesheet.width(), 0.5f / tilesheet.height());
	for (int j = 0; j < mapSize.y; j++)
	{
		for (int i = 0; i < mapSize.x; i++)
		{
			tile = map[j * mapSize.x + i];
			if (tile != 0)
			{
				// Non-empty tile
				nTiles++;
				posTile = glm::vec2(minCoords.x + i * tileSize, minCoords.y + j * tileSize);
				texCoordTile[0] = glm::vec2(float((tile - 1) % tilesheetSize.x) / tilesheetSize.x, float((tile - 1) / tilesheetSize.x) / tilesheetSize.y);
				texCoordTile[1] = texCoordTile[0] + tileTexSize;
				// texCoordTile[0] += halfTexel;
				texCoordTile[1] -= halfTexel;
				// First triangle
				vertices.push_back(posTile.x);
				vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[0].x);
				vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize);
				vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[1].x);
				vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize);
				vertices.push_back(posTile.y + blockSize);
				vertices.push_back(texCoordTile[1].x);
				vertices.push_back(texCoordTile[1].y);
				// Second triangle
				vertices.push_back(posTile.x);
				vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[0].x);
				vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize);
				vertices.push_back(posTile.y + blockSize);
				vertices.push_back(texCoordTile[1].x);
				vertices.push_back(texCoordTile[1].y);
				vertices.push_back(posTile.x);
				vertices.push_back(posTile.y + blockSize);
				vertices.push_back(texCoordTile[0].x);
				vertices.push_back(texCoordTile[1].y);
			}
		}
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * nTiles * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	posLocation = program.bindVertexAttribute("position", 2, 4 * sizeof(float), 0);
	texCoordLocation = program.bindVertexAttribute("texCoord", 2, 4 * sizeof(float), (void *)(2 * sizeof(float)));
}

// Collision tests for axis aligned bounding boxes.
// Method collisionMoveDown also corrects Y coordinate if the box is
// already intersecting a tile below.

bool TileMap::collisionMoveLeft(const glm::ivec2 &pos, const glm::ivec2 &size, const bool &bJumping) const
{
	int x, y0, y1;

	x = pos.x / tileSize;
	y0 = pos.y / tileSize;
	y1 = (pos.y + size.y - 1) / tileSize;
	for (int y = y0; y <= y1; y++)
	{
		const int tile = map[y * mapSize.x + x];
		if (isCollisionTile(tile) or (not bJumping and isChangeableTile(tile)))
			return true;
	}

	return false;
}

bool TileMap::collisionMoveRight(const glm::ivec2 &pos, const glm::ivec2 &size, const bool &bJumping) const
{
	int x, y0, y1;

	x = (pos.x + size.x - 1) / tileSize;
	y0 = pos.y / tileSize;
	y1 = (pos.y + size.y - 1) / tileSize;
	for (int y = y0; y <= y1; y++)
	{
		const int tile = map[y * mapSize.x + x];
		if (isCollisionTile(tile) or (not bJumping and isChangeableTile(tile)))
			return true;
	}

	return false;
}

bool TileMap::collisionMoveUp(const glm::ivec2 &pos, const glm::ivec2 &size) const
{
	int x0, x1, y;

	x0 = pos.x / tileSize;
	x1 = (pos.x + size.x - 1) / tileSize;
	y = pos.y / tileSize;
	for (int x = x0; x <= x1; x++)
	{
		const int tile = map[y * mapSize.x + x];
		if (isCollisionTile(tile))
		{
			return true;
		}
	}

	return false;
}

bool TileMap::collisionMoveDown(const glm::ivec2 &pos, const glm::ivec2 &size, int *posY)
{
	int x0, x1, y;

	bool result = false;

	x0 = pos.x / tileSize;
	x1 = (pos.x + size.x - 1) / tileSize;
	y = (pos.y + size.y - 1) / tileSize;
	for (int x = x0; x <= x1; x++)
	{
		const int tile = map[y * mapSize.x + x];
		if (isCollisionTile(tile) or isChangeableTile(tile) or tile == SPIKES)
		{
			if (*posY - tileSize * y + size.y <= 4)
			{
				*posY = tileSize * y - size.y;

				checkCollisionChangeableTile(x, y);

				result = true;
			}
		}
	}

	return result;
}

void TileMap::checkCollisionChangeableTile(int tileX, int tileY)
{
	auto it = changeableTiles.find({tileY, tileX});
	if (it != changeableTiles.end())
	{
		it->second = true;
	}
}

bool TileMap::isCompleted() const
{
	for (const auto &[_, pressed] : changeableTiles)
	{
		if (!pressed)
		{
			return false;
		}
	}

	return true;
}

StaticSprite *TileMap::getActiveChangeableTile(const int tile) const
{
	const int updated = tile - CHANGEABLE_PLATFORM_RANGE_START;
	assert(updated >= 0 && updated < changeableToActive.size());
	return changeableToActive[updated];
}