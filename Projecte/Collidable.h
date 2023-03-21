#pragma once

#include <glm/glm.hpp>
#include <iostream>

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

class Collidable
{
public:
    virtual ~Collidable() {}

    virtual glm::ivec2 getPosition() const = 0;
    virtual BoundingBoxInfo getBoundingBoxInfo() const = 0;

    bool isColliding(const Collidable &other) const
    {
        const auto ownBoundingBox = getBoundingBoxInfo();
        const auto otherBoundingBox = other.getBoundingBoxInfo();

        const auto &ownPosition = getPosition();
        const auto &otherPosition = other.getPosition();

        BoundingBox AABB{};
        AABB.x = ownPosition.x + ownBoundingBox.xoffset;
        AABB.y = ownPosition.y + ownBoundingBox.yoffset;
        AABB.w = ownBoundingBox.width;
        AABB.h = ownBoundingBox.height;

        BoundingBox otherAABB{};
        otherAABB.x = otherPosition.x + otherBoundingBox.xoffset;
        otherAABB.y = otherPosition.y + otherBoundingBox.yoffset;
        otherAABB.w = otherBoundingBox.width;
        otherAABB.h = otherBoundingBox.height;

        return AABB.x < otherAABB.x + otherAABB.w &&
               AABB.x + AABB.w > otherAABB.x &&
               AABB.y < otherAABB.y + otherAABB.h &&
               AABB.h + AABB.y > otherAABB.y;
    }
};