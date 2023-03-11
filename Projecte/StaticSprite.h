#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "Texture.h"
#include "ShaderProgram.h"
#include "AnimKeyframes.h"

class StaticSprite
{
private:
    StaticSprite(const glm::vec2 &quadSize, const glm::vec2 &sizeInSpritesheet, Texture *spritesheet, ShaderProgram *program);

public:
    static StaticSprite *createSprite(const glm::vec2 &quadSize, const glm::vec2 &sizeInSpritesheet, Texture *spritesheet, ShaderProgram *program);

    void render() const;
    void free();

    void setPosition(const glm::vec2 &pos);
    void setSpritesheetCoords(const glm::vec2 &coords);

private:
    Texture *texture;
    ShaderProgram *shaderProgram;
    GLuint vao;
    GLuint vbo;

    GLint posLocation, texCoordLocation;
    glm::vec2 position;
    glm::vec2 texCoordDispl;
};