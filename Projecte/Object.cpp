#include "Object.h"

#include <algorithm>

//
// Door
//

Door::Door(Texture *tileset, const glm::ivec2 &positionTop, const glm::ivec2 &positionBottom, ShaderProgram *program)
{
    position = positionTop;

    texture = new Texture();
    texture->loadFromFile("images/door.png", PixelFormat::TEXTURE_PIXEL_FORMAT_RGBA);

    sprite = AnimatedSprite::createSprite(glm::vec2(48.0f), glm::vec2(1.0f / 4.0f, 1.0f), texture, program);
    sprite->setPosition(positionTop);

    sprite->setNumberAnimations(3);

    // Closed
    sprite->setAnimationSpeed(0, 8);
    sprite->addKeyframe(0, glm::vec2(0.0f / 4.0f, 0.0f));

    constexpr int openingDoorKeyframesPerSecond = 2;
    // milliseconds for the door to open
    timeToOpenDoor_ms = (1.0f / float(openingDoorKeyframesPerSecond)) * 4.0f * 1000.0f;

    sprite->setAnimationSpeed(1, openingDoorKeyframesPerSecond);
    sprite->addKeyframe(1, glm::vec2(0.0f / 4.0f, 0.0f));
    sprite->addKeyframe(1, glm::vec2(1.0f / 4.0f, 0.0f));
    sprite->addKeyframe(1, glm::vec2(2.0f / 4.0f, 0.0f));
    sprite->addKeyframe(1, glm::vec2(3.0f / 4.0f, 0.0f));

    sprite->setAnimationSpeed(2, 8);
    sprite->addKeyframe(2, glm::vec2(3.0f / 4.0f, 0.0f));

    sprite->changeAnimation(0);
}

Door::~Door()
{
    delete sprite;
    delete texture;
}

void Door::render() const
{
    sprite->render();
}

void Door::update(int deltaTime)
{
    if (sprite->animation() == 1)
        timeOpening += deltaTime;

    if (sprite->animation() == 1 && timeOpening >= timeToOpenDoor_ms)
        sprite->changeAnimation(2);

    sprite->update(deltaTime);
}

void Door::open() const
{
    sprite->changeAnimation(1);
}

BoundingBoxInfo Door::getBoundingBoxInfo() const
{
    return {
        .xoffset = 5,
        .yoffset = 2,
        .width = 6,
        .height = 26};
}

//
// Key
//

Key::Key(Texture *tileset, const glm::ivec2 &pos, ShaderProgram *program)
{
    position = pos;
    sprite = StaticSprite::createSprite(glm::vec2(20.0f), glm::vec2(1.0f / 10.0f, 1.0f / 10.0f), tileset, program);
    sprite->setPosition(position);
    sprite->setSpritesheetCoords(glm::vec2(0.0f, 4.0f / 10.0f));
}

Key::~Key()
{
    delete sprite;
}

void Key::render() const
{
    sprite->render();
}

BoundingBoxInfo Key::getBoundingBoxInfo() const
{
    return {
        .xoffset = 4,
        .yoffset = 0,
        .width = 12,
        .height = 14};
}

//
// Gem
//

Gem::Gem(Texture *tileset, const glm::ivec2 &pos, ShaderProgram *program)
{
    position = pos;
    sprite = StaticSprite::createSprite(glm::vec2(20.0f), glm::vec2(1.0f / 10.0f, 1.0f / 10.0f), tileset, program);
    sprite->setPosition(position);
    sprite->setSpritesheetCoords(glm::vec2(6.0f / 10.0f, 4.0f / 10.0f));
}

Gem::~Gem()
{
    delete sprite;
}

void Gem::render() const
{
    sprite->render();
}

BoundingBoxInfo Gem::getBoundingBoxInfo() const
{
    return {
        .xoffset = 4,
        .yoffset = 0,
        .width = 12,
        .height = 14};
}

//
// Clock
//

Clock::Clock(Texture *tileset, const glm::ivec2 &pos, ShaderProgram *program)
{
    position = pos;

    texture = new Texture();
    texture->loadFromFile("images/clock.png", PixelFormat::TEXTURE_PIXEL_FORMAT_RGBA);

    sprite = AnimatedSprite::createSprite(glm::vec2(14.0f, 20.0f), glm::vec2(1.0f / 4.0f, 1.0f), texture, program);
    sprite->setPosition(position);

    sprite->setNumberAnimations(1);
    sprite->setAnimationSpeed(0, 4);
    sprite->addKeyframe(0, glm::vec2(0.0f / 4.0f, 0.0f));
    sprite->addKeyframe(0, glm::vec2(1.0f / 4.0f, 0.0f));
    sprite->addKeyframe(0, glm::vec2(2.0f / 4.0f, 0.0f));
    sprite->addKeyframe(0, glm::vec2(3.0f / 4.0f, 0.0f));

    sprite->changeAnimation(0);
}

Clock::~Clock()
{
    delete texture;
    delete sprite;
}

void Clock::render() const
{
    sprite->render();
}

void Clock::update(int deltaTime)
{
    sprite->update(deltaTime);
}

BoundingBoxInfo Clock::getBoundingBoxInfo() const
{
    return {
        .xoffset = 4,
        .yoffset = 0,
        .width = 12,
        .height = 14};
}

//
// Shield
//
Shield::Shield(Texture *tileset, const glm::ivec2 &pos, ShaderProgram *program)
{
    position = pos;

    texture = new Texture();
    texture->loadFromFile("images/shield.png", PixelFormat::TEXTURE_PIXEL_FORMAT_RGBA);

    sprite = AnimatedSprite::createSprite(glm::vec2(20.0f, 20.0f), glm::vec2(1.0f / 2.0f, 1.0f), texture, program);
    sprite->setPosition(position);

    sprite->setNumberAnimations(1);
    sprite->setAnimationSpeed(0, 2);
    sprite->addKeyframe(0, glm::vec2(0.0f / 2.0f, 0.0f));
    sprite->addKeyframe(0, glm::vec2(1.0f / 2.0f, 0.0f));

    sprite->changeAnimation(0);
}

Shield::~Shield()
{
    delete texture;
    delete sprite;
}

void Shield::render() const
{
    sprite->render();
}

void Shield::update(int deltaTime)
{
    sprite->update(deltaTime);
}

BoundingBoxInfo Shield::getBoundingBoxInfo() const
{
    return {
        .xoffset = 4,
        .yoffset = 0,
        .width = 12,
        .height = 14};
}