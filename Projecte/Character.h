#pragma once

#include "AnimatedSprite.h"
#include "TileMap.h"

enum CharacterAnims
{
    STAND_LEFT = 0,
    STAND_RIGHT = 1,
    MOVE_LEFT = 2,
    MOVE_RIGHT = 3
};

struct BoundingBox
{
    int x;
    int y;
    int w;
    int h;
};

struct BoundingBoxInfo
{
    int xoffset;
    int yoffset;
    int width;
    int height;
};

#define FALL_STEP 4

class Character
{
public:
    virtual void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram) = 0;
    virtual void update(int deltaTime) = 0;
    void render() const { sprite->render(); }

    void setTileMap(TileMap *tileMap) { map = tileMap; }
    void setPosition(const glm::vec2 &pos) { position = pos; }

    const glm::ivec2 &getPosition() const { return position; }

    BoundingBox getBoundingBox() const
    {
        BoundingBox AABB{};
        AABB.x = position.x + boundingBoxInfo.xoffset;
        AABB.y = position.y + boundingBoxInfo.yoffset;
        AABB.w = boundingBoxInfo.width;
        AABB.h = boundingBoxInfo.height;

        return AABB;
    }

    bool isColliding(const Character &other) const
    {
        const auto AABB = getBoundingBox();
        const auto otherAABB = other.getBoundingBox();

        return AABB.x < otherAABB.x + otherAABB.w &&
               AABB.x + AABB.w > otherAABB.x &&
               AABB.y < otherAABB.y + otherAABB.h &&
               AABB.h + AABB.y > otherAABB.y;
    }

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