#pragma once

class IGameScene
{
public:
    virtual ~IGameScene() {}

    virtual int update(int deltaTime) = 0;
    virtual void render() = 0;
};