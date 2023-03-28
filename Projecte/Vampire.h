#pragma once

#include "Sprite.h"
#include "TileMap.h"

#include "Enemy.h"

#define TIME_PER_STAGE 10 // seconds

class Vampire : public Enemy
{
public:
    ~Vampire() override;

    void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram) override;
    void update(int deltaTime) override;

    virtual BoundingBoxInfo getBoundingBoxInfo() const override;

private:
    void updateFlying(int deltaTime);
    void updateWalking(int deltaTime);
    void updateLanding(int deltaTime);

    Texture *vampireTexture;
    Texture *batTexture;

    AnimatedSprite *vampireSprite;
    AnimatedSprite *batSprite;

    bool flying = false;
    bool landing = false;
    int timeSinceLastFly_ms;
    glm::ivec2 flyingMovement;
};