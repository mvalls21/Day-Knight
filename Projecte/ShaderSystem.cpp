#include "ShaderSystem.h"

#include <iostream>

using namespace std;

std::unordered_map<std::string, ShaderProgram *> ShaderSystem::shaders = {};

void ShaderSystem::init()
{
}

void ShaderSystem::free()
{
    for (auto &pair : shaders)
    {
        delete pair.second;
    }
}

ShaderProgram *ShaderSystem::acquire(const std::string &shaderName)
{
    if (shaders.find(shaderName) != shaders.end())
        return shaders[shaderName];

    auto *program = loadShader(shaderName);
    shaders[shaderName] = program;

    return program;
}

ShaderProgram *ShaderSystem::loadShader(const std::string &shaderName)
{
    Shader vShader, fShader;

    const std::string vertexShader = "shaders/" + shaderName + ".vert";
    const std::string fragmentShader = "shaders/" + shaderName + ".frag";

    vShader.initFromFile(VERTEX_SHADER, vertexShader);
    if (!vShader.isCompiled())
    {
        cout << "Vertex Shader Error" << endl;
        cout << "" << vShader.log() << endl
             << endl;
    }
    fShader.initFromFile(FRAGMENT_SHADER, fragmentShader);
    if (!fShader.isCompiled())
    {
        cout << "Fragment Shader Error" << endl;
        cout << "" << fShader.log() << endl
             << endl;
    }

    auto *program = new ShaderProgram();

    program->init();
    program->addShader(vShader);
    program->addShader(fShader);
    program->link();
    if (!program->isLinked())
    {
        cout << "Shader Linking Error" << endl;
        cout << "" << program->log() << endl
             << endl;
    }

    program->bindFragmentOutput("outColor");
    vShader.free();
    fShader.free();

    return program;
}
