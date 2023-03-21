#pragma once

#include "Collidable.h"

#include "AnimatedSprite.h"
#include "StaticSprite.h"

class Object : public Collidable
{
public:
    virtual ~Object() {}

    virtual void render() const = 0;
    glm::ivec2 getPosition() const override { return position; }

protected:
    glm::ivec2 position;
};

class Door : public Object
{
public:
    Door(Texture *tileset, const glm::ivec2 &positionTop, const glm::ivec2 &positionBottom, ShaderProgram *program);
    ~Door() override;

    void render() const override;
    void open() const;

    BoundingBoxInfo getBoundingBoxInfo() const override;

private:
    std::vector<AnimatedSprite *> sprites;
};

class Key : public Object
{
public:
    Key(Texture *tileset, const glm::ivec2 &pos, ShaderProgram *program);
    ~Key() override;

    void render() const override;

    BoundingBoxInfo getBoundingBoxInfo() const override;

private:
    StaticSprite *sprite;
};

class Gem : public Object
{
public:
    Gem(Texture *tileset, const glm::ivec2 &pos, ShaderProgram *program);
    ~Gem() override;

    void render() const override;

    BoundingBoxInfo getBoundingBoxInfo() const override;

private:
    StaticSprite *sprite;
};

class Clock : public Object
{
public:
    Clock(Texture *tileset, const glm::ivec2 &pos, ShaderProgram *program);
    ~Clock();

    void render() const override;

    BoundingBoxInfo getBoundingBoxInfo() const override;

private:
    StaticSprite *sprite;
};