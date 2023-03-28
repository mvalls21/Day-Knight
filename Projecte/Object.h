#pragma once

#include "Collidable.h"

#include "AnimatedSprite.h"
#include "StaticSprite.h"

class Object : public Collidable
{
public:
    virtual ~Object() {}

    virtual void render() const = 0;
    virtual void update(int deltaTime) {}
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
    void update(int deltaTime) override;
    void open() const;

    BoundingBoxInfo getBoundingBoxInfo() const override;

private:
  AnimatedSprite* sprite;
  Texture* texture;

  int timeOpening = 0;
  int openingTimeout;
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
    void update(int deltaTime) override;

    BoundingBoxInfo getBoundingBoxInfo() const override;

private:
    AnimatedSprite *sprite;
    Texture *texture;
};

class Shield : public Object
{
public:
    Shield(Texture *tileset, const glm::ivec2 &pos, ShaderProgram *program);
    ~Shield();

    void render() const override;
    void update(int deltaTime) override;
    void setPosition(const glm::ivec2 &pos)
    {
        position = pos;
        sprite->setPosition(pos);
    }

    BoundingBoxInfo getBoundingBoxInfo() const override;

private:
    AnimatedSprite *sprite;
    Texture *texture;
};