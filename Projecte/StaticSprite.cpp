#include "StaticSprite.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/gtc/matrix_transform.hpp>

StaticSprite *StaticSprite::createSprite(const glm::vec2 &quadSize, const glm::vec2 &sizeInSpritesheet, Texture *spritesheet, ShaderProgram *program)
{
    return new StaticSprite(quadSize, sizeInSpritesheet, spritesheet, program);
}

StaticSprite::StaticSprite(const glm::vec2 &quadSize, const glm::vec2 &sizeInSpritesheet, Texture *spritesheet, ShaderProgram *program)
    : Sprite(quadSize, sizeInSpritesheet, spritesheet, program)
{
}

void StaticSprite::update(int deltaTime)
{
    // Does nothing
}

void StaticSprite::setSpritesheetCoords(const glm::vec2 &coords)
{
    texCoordDispl = coords;
}