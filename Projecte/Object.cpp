#include "Object.h"

#include <algorithm>

//
// Door
//

Door::Door(Texture *tileset, const glm::ivec2 &positionTop, const glm::ivec2 &positionBottom, ShaderProgram *program)
{
    position = positionTop;

    auto doorSprite1 = AnimatedSprite::createSprite(glm::vec2(16.0f), glm::vec2(1.0f / 10.0f, 1.0f / 10.0f), tileset, program);
    doorSprite1->setPosition(positionBottom);

    doorSprite1->setNumberAnimations(2);
    doorSprite1->addKeyframe(0, {0.0f / 10.0f, 3.0f / 10.0f});
    doorSprite1->addKeyframe(1, {2.0f / 10.0f, 3.0f / 10.0f});

    doorSprite1->changeAnimation(0);

    auto doorSprite2 = AnimatedSprite::createSprite(glm::vec2(16.0f), glm::vec2(1.0f / 10.0f, 1.0f / 10.0f), tileset, program);
    doorSprite2->setPosition(positionTop);

    doorSprite2->setNumberAnimations(2);
    doorSprite2->addKeyframe(0, {1.0f / 10.0f, 3.0f / 10.0f});
    doorSprite2->addKeyframe(1, {3.0f / 10.0f, 3.0f / 10.0f});

    doorSprite2->changeAnimation(0);

    sprites.push_back(doorSprite1);
    sprites.push_back(doorSprite2);
}

Door::~Door()
{
    for (const AnimatedSprite *sprite : sprites)
        delete sprite;
}

void Door::render() const
{
    for (const auto &sprite : sprites)
        sprite->render();
}

void Door::open() const
{
    for (const auto &sprite : sprites)
        sprite->changeAnimation(1);
}

BoundingBoxInfo Door::getBoundingBoxInfo() const
{
    return {
        .xoffset = -16 + 5,
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
        .xoffset = -20 + 4,
        .yoffset = 0,
        .width = 12,
        .height = 14};
}