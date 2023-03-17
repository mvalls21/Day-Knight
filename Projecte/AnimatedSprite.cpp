#include "AnimatedSprite.h"

AnimatedSprite *AnimatedSprite::createSprite(const glm::vec2 &quadSize, const glm::vec2 &sizeInSpritesheet, Texture *spritesheet, ShaderProgram *program)
{
    AnimatedSprite *quad = new AnimatedSprite(quadSize, sizeInSpritesheet, spritesheet, program);

    return quad;
}

AnimatedSprite::AnimatedSprite(const glm::vec2 &quadSize, const glm::vec2 &sizeInSpritesheet, Texture *spritesheet, ShaderProgram *program)
    : Sprite(quadSize, sizeInSpritesheet, spritesheet, program)
{
    currentAnimation = -1;
}

void AnimatedSprite::update(int deltaTime)
{
    if (currentAnimation >= 0)
    {
        timeAnimation += deltaTime;
        while (timeAnimation > animations[currentAnimation].millisecsPerKeyframe)
        {
            timeAnimation -= animations[currentAnimation].millisecsPerKeyframe;
            currentKeyframe = (currentKeyframe + 1) % animations[currentAnimation].keyframeDispl.size();
        }
        texCoordDispl = animations[currentAnimation].keyframeDispl[currentKeyframe];
    }
}

void AnimatedSprite::setNumberAnimations(int nAnimations)
{
    animations.clear();
    animations.resize(nAnimations);
}

void AnimatedSprite::setAnimationSpeed(int animId, int keyframesPerSec)
{
    if (animId < int(animations.size()))
        animations[animId].millisecsPerKeyframe = 1000.f / keyframesPerSec;
}

void AnimatedSprite::addKeyframe(int animId, const glm::vec2 &displacement)
{
    if (animId < int(animations.size()))
        animations[animId].keyframeDispl.push_back(displacement);
}

void AnimatedSprite::changeAnimation(int animId)
{
    if (animId < int(animations.size()))
    {
        currentAnimation = animId;
        currentKeyframe = 0;
        timeAnimation = 0.f;
        texCoordDispl = animations[animId].keyframeDispl[0];
    }
}

int AnimatedSprite::animation() const
{
    return currentAnimation;
}