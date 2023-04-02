#pragma once

#include "Enemy.h"

class Ghost : public Enemy
{
public:
    void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram) override;
    void update(int deltaTime) override;

    virtual BoundingBoxInfo getBoundingBoxInfo() const override;

private:
    glm::vec2 flyingMovement = {0.7f, 0.7f};
};