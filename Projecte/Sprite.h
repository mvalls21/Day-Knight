#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "Texture.h"
#include "ShaderProgram.h"
#include "AnimKeyframes.h"

// This class is derived from code seen earlier in TexturedQuad but it is also
// able to manage animations stored as a spritesheet.

class Sprite
{
protected:
	Sprite(const glm::vec2 &quadSize, const glm::vec2 &sizeInSpritesheet, Texture *spritesheet, ShaderProgram *program);

public:
	virtual void update(int deltaTime) = 0;
	virtual void render() const;
	void free();

	void setPosition(const glm::vec2 &pos);

protected:
	Texture *texture;
	ShaderProgram *shaderProgram;
	GLuint vao;
	GLuint vbo;
	GLint posLocation;
	GLint texCoordLocation;

	glm::vec2 position;
	glm::vec2 texCoordDispl;
};
