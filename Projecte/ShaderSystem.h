#pragma once

#include <unordered_map>

#include "ShaderProgram.h"

class ShaderSystem
{
public:
    static void init();
    static void free();

    static ShaderProgram *acquire(const std::string &shaderName);

private:
    static std::unordered_map<std::string, ShaderProgram *> shaders;

    static ShaderProgram *loadShader(const std::string &shaderName);
};