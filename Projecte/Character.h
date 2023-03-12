#pragma once

#include "Sprite.h"
#include "TileMap.h"

enum CharacterAnims
{
    STAND_LEFT = 0,
    STAND_RIGHT = 1,
    MOVE_LEFT = 2,
    MOVE_RIGHT = 3
};

class Character
{
public:
    virtual void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram) = 0;
    virtual void update(int deltaTime) = 0;
    void render() const { sprite->render(); }

    void setTileMap(TileMap *tileMap) { map = tileMap; }
    void setPosition(const glm::vec2 &pos) { position = pos; }

    const glm::ivec2 &getPosition() const { return position; }

protected:
    glm::ivec2 tileMapDispl;
    glm::ivec2 position;

    Texture spritesheet;
    Sprite *sprite;
    TileMap *map;
};