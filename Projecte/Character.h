#pragma once

#include "Collidable.h"

#include "AnimatedSprite.h"
#include "TileMap.h"

enum CharacterAnims
{
    STAND_LEFT = 0,
    STAND_RIGHT = 1,
    MOVE_LEFT = 2,
    MOVE_RIGHT = 3
};

#define FALL_STEP 4

class Character : public Collidable
{
public:
    virtual void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram) = 0;
    virtual void update(int deltaTime) = 0;
    virtual void render() const { sprite->render(); }

    void setTileMap(TileMap *tileMap) { map = tileMap; }
    void setPosition(const glm::vec2 &pos) { position = pos; }

    virtual glm::ivec2 getPosition() const override
    {
        return {float(tileMapDispl.x + position.x), float(tileMapDispl.y + position.y)};
    }

    virtual BoundingBoxInfo getBoundingBoxInfo() const override { return boundingBoxInfo; }

protected:
    glm::ivec2 tileMapDispl;
    glm::ivec2 position;
    BoundingBoxInfo boundingBoxInfo{
        .xoffset = 3,
        .yoffset = 3,
        .width = 24,
        .height = 24};

    Texture spritesheet;
    AnimatedSprite *sprite;
    TileMap *map;
};