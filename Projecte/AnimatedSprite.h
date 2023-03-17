#pragma once

#include "Sprite.h"

class AnimatedSprite : public Sprite
{
private:
    AnimatedSprite(const glm::vec2 &quadSize, const glm::vec2 &sizeInSpritesheet, Texture *spritesheet, ShaderProgram *program);

public:
    // Textured quads can only be created inside an OpenGL context
    static AnimatedSprite *createSprite(const glm::vec2 &quadSize, const glm::vec2 &sizeInSpritesheet, Texture *spritesheet, ShaderProgram *program);

    void update(int deltaTime) override;

    void setNumberAnimations(int nAnimations);
    void setAnimationSpeed(int animId, int keyframesPerSec);
    void addKeyframe(int animId, const glm::vec2 &frame);
    void changeAnimation(int animId);
    int animation() const;

private:
    int currentAnimation;
    int currentKeyframe;

    float timeAnimation;
    vector<AnimKeyframes> animations;
};
