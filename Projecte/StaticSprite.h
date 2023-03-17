#pragma once

#include "Sprite.h"

class StaticSprite : public Sprite
{
private:
    StaticSprite(const glm::vec2 &quadSize, const glm::vec2 &sizeInSpritesheet, Texture *spritesheet, ShaderProgram *program);

public:
    static StaticSprite *createSprite(const glm::vec2 &quadSize, const glm::vec2 &sizeInSpritesheet, Texture *spritesheet, ShaderProgram *program);

    void update(int deltaTime) override;
    void setSpritesheetCoords(const glm::vec2 &coords);
};