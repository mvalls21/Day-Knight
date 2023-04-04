#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include "Scene.h"
#include "Game.h"
#include "ShaderSystem.h"

#include <random>

#define SCREEN_X 26
#define SCREEN_Y 46

#define INIT_PLAYER_X_TILES 8
#define INIT_PLAYER_Y_TILES 3

#define TILE_POS(x, y) glm::vec2(SCREEN_X + (x)*map->getTileSize(), SCREEN_Y + (y)*map->getTileSize())

Scene::Scene()
{
    map = NULL;
    player = NULL;

    text = new Text();
    if (!text->init("../Projecte/fonts/OldLondon.ttf"))
        assert(false && "tonto");
}

Scene::~Scene()
{
    delete map;
    delete tileset;

    delete player;
    for (const auto *enemy : enemies)
        delete enemy;

    delete key;
    delete door;
    delete heart;

    if (currentObject != nullptr)
        delete currentObject;

    delete text;
}

void Scene::init(const Description &description)
{
    texProgram = ShaderSystem::acquire("texture");

    projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f);
    projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1) / 2.4f, float(SCREEN_HEIGHT - 1) / 2.4f, 0.f);
    currentTime = 0.0f;

    // 	Load level
    std::string levelPath = "levels/" + description.levelName;
    map = TileMap::createTileMap(levelPath, {SCREEN_X, SCREEN_Y}, *texProgram, description.score);

    // Create player
    player = new Player();
    player->init(glm::ivec2(SCREEN_X, SCREEN_Y), *texProgram);
    player->setPosition(glm::vec2(description.playerPositionStartTile.x * map->getTileSize(), description.playerPositionStartTile.y * map->getTileSize()));
    player->setTileMap(map);

    // Create skeletons
    for (const auto &desc : description.skeletonDescriptions)
    {
        auto *skeleton = new Skeleton();
        skeleton->init(glm::ivec2(SCREEN_X, SCREEN_Y), *texProgram);
        skeleton->setTileMap(map);
        skeleton->setPosition({desc.tileX * map->getTileSize(), desc.tileY * map->getTileSize()});
        skeleton->setDirection(desc.startingDirection);

        enemies.push_back(skeleton);
    }

    // Create vampires
    for (const auto &desc : description.vampireDescriptions)
    {
        auto *vampire = new Vampire();
        vampire->init(glm::ivec2(SCREEN_X, SCREEN_Y), *texProgram);
        vampire->setTileMap(map);
        vampire->setPosition({desc.tileX * map->getTileSize(), desc.tileY * map->getTileSize()});
        vampire->setDirection(desc.startingDirection);

        enemies.push_back(vampire);
    }

    // Create ghosts
    for (const auto &desc : description.ghostDescriptions)
    {
        auto *ghost = new Ghost();
        ghost->init(glm::ivec2(SCREEN_X, SCREEN_Y), *texProgram);
        ghost->setTileMap(map);
        ghost->setPosition({desc.tileX * map->getTileSize(), desc.tileY * map->getTileSize()});
        ghost->setDirection(desc.startingDirection);

        enemies.push_back(ghost);
    }

    tileset = new Texture();
    tileset->loadFromFile("images/nuevo_tileset.png", PixelFormat::TEXTURE_PIXEL_FORMAT_RGBA);

    heart = StaticSprite::createSprite(glm::vec2(20.f), glm::vec2(1.f / 10.f), tileset, texProgram);
    heart->setSpritesheetCoords(glm::vec2(5.f / 10.f, 4.f / 10.f));

    key = new Key(tileset, {SCREEN_X + description.keyPositionTile.x * map->getTileSize(), SCREEN_Y + description.keyPositionTile.y * map->getTileSize()}, texProgram);

    // const glm::ivec2 doorPositionTop = TILE_POS(description.doorPositionTile.x, description.doorPositionTile.y - 1);
    const glm::ivec2 doorPositionTop = TILE_POS(description.doorPositionTile.x, description.doorPositionTile.y - 2);

    const glm::ivec2 doorPositionBottom = TILE_POS(description.doorPositionTile.x, description.doorPositionTile.y);
    door = new Door(tileset, doorPositionTop, doorPositionBottom, texProgram);

    // Create objects with random order
    std::random_device r;
    std::default_random_engine engine(r());

    std::vector<ObjectType> objects = {ObjectType::Gem, ObjectType::Clock, ObjectType::Shield};
    std::shuffle(objects.begin(), objects.end(), engine);

    for (const auto &object : objects)
        remainingObjectTypes.push(object);

    // Shield sprite for hearts
    shield = new Shield(tileset, TILE_POS(0, 0), texProgram);

    stageNum = description.stageNumber;
    score = description.score;

    // First update to position everything in its place
    update(-1);
}

SceneStatus Scene::update(int deltaTime)
{
    currentTime += deltaTime;

    if (deltaTime != -1 && currentTime < TIME_TO_START)
    {
        return SceneStatus::Continue;
    }

    levelTimer -= deltaTime;
    if (levelTimer <= 0)
    {
        return SceneStatus::PlayerDead;
    }


    detectCheatKeys();
    objectTimer += deltaTime;

    map->update(deltaTime);

    player->update(deltaTime);

    for (auto *enemy : enemies)
        enemy->update(deltaTime);

    bool enemyCollision = !player->isImmune() && std::any_of(enemies.begin(), enemies.end(), [&](const Enemy *enemy)
                                                             { return player->isColliding(*enemy); });

    if (enemyCollision && !invulnerability)
    {
        if (shieldProtection)
            shieldProtection = false;
        else
            player->setLives(player->getLives() - 1);

        if (player->getLives() != 0)
            player->makeImmune(PLAYER_IMMUNITY_MS);
    }

    if (player->getLives() == 0 && player->finishedDeath())
    {
        return SceneStatus::PlayerDead;
    }
    else if (player->getLives() == 0)
    {
        return SceneStatus::Continue;
    }

    // Object stuff (key, door, powerups...)

    if (!showKey && !isDoorOpen && map->isCompleted())
        showKey = true;

    if (showKey && !isDoorOpen)
    {
        if (player->isColliding(*key))
        {
            isDoorOpen = true;
            showKey = false;
            door->open();
        }
    }

    door->update(deltaTime);

    if (isDoorOpen && player->isColliding(*door))
        return SceneStatus::LevelComplete;

    if (currentObjectType != ObjectType::None && currentObject->isColliding(*player))
    {
        switch (currentObjectType)
        {
        case ObjectType::Gem:
            *score += GEM_SCORE_BONUS;
            break;
        case ObjectType::Clock:
            levelTimer += TIME_PRIZE_CLOCK_OBJ;
            break;
        case ObjectType::Shield:
            // TODO: Should also give immunity for some time for player, but without effect...
            shieldProtection = true;
            break;
        default:
            break;
        }
        currentObjectType = ObjectType::None;
    }

    // Object spawn logic
    if (currentObjectType != ObjectType::None)
        currentObject->update(deltaTime);

    if (objectTimer >= OBJECT_PERIOD)
    {
        // ObjectType::None means that player should not interact with object, but existance or not is controlled
        // by currentObject == nullptr
        objectTimer = 0;

        if (currentObject == nullptr)
        {
            spawnRandomObject();
        }
        else
        {
            delete currentObject;
            currentObject = nullptr;
            currentObjectType = ObjectType::None;
        }
    }

    return SceneStatus::Continue;
}

void Scene::render()
{
    glm::mat4 modelview;

    texProgram->use();

    texProgram->setUniformMatrix4f("projection", projection);
    texProgram->setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
    modelview = glm::mat4(1.0f);
    texProgram->setUniformMatrix4f("modelview", modelview);
    texProgram->setUniform2f("texCoordDispl", 0.f, 0.f);

    map->render();
    door->render();

    for (int i = 0; i < player->getLives(); ++i)
    {
        if (i == player->getLives() - 1 && shieldProtection)
        {
            shield->setPosition(glm::vec2(SCREEN_X + 20 * i, SCREEN_Y - 30));
            shield->render();
            continue;
        }

        heart->setPosition(glm::vec2(SCREEN_X + 20 * i, SCREEN_Y - 30));
        heart->render();
    }

    for (const auto *enemy : enemies)
        enemy->render();

    if (showKey)
        key->render();

    if (currentObjectType != ObjectType::None)
        currentObject->render();

    player->render();

    text->render(std::to_string((levelTimer/1000)/10) + std::to_string((levelTimer/1000)%10), glm::vec2(SCREEN_X +625, SCREEN_Y + 35), 60.0f,
                 levelTimer >= 10000 ? glm::vec4(1.0f) : glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    string stageNumString = string(1, '0' + (stageNum / 10) % 10) + string(1, '0' + (stageNum % 10));
    text->render("Stage " + stageNumString, glm::vec2(SCREEN_X + 1050, SCREEN_Y + 35), 60.0f, glm::vec4(1.0f));

    string scoreString = string(1, '0' + (*score / 10000) % 10) + string(1, '0' + (*score / 1000) % 10) +
            string(1, '0' + (*score / 100) % 10) + string(1, '0' + (*score / 10) % 10) + string(1, '0' + (*score) % 10);
    text->render(scoreString, glm::vec2(SCREEN_X + 325, SCREEN_Y + 35), 60.0f, glm::vec4(1.0f));

    if (currentTime < TIME_TO_START)
    {
        // TODO: Render start text
        int dimensions[4];
        glGetIntegerv(GL_VIEWPORT, dimensions);

        constexpr float size = 70.0f;

        float middleX = (float)dimensions[2] / 2.0f - 84.0f;
        float middleY = (float)dimensions[3] / 2.0f;

        const std::string content("Ready?");
        text->render(content, glm::vec2(middleX, middleY), size, glm::vec4(1.0f, 1.0f, 1.0f, 0.8f));
    }
}

void Scene::spawnRandomObject()
{
    if (remainingObjectTypes.size() == 0)
        return;

    std::random_device r;
    std::default_random_engine engine(r());

    constexpr glm::vec2 offset = {0, 7};

    // Select platform
    const auto platforms = map->getPlatforms();
    std::uniform_int_distribution<int> platform_random(0, platforms.size() - 1);
    const int platformIdx = platform_random(engine);

    const auto platform = platforms[platformIdx];
    const auto pos = TILE_POS(platform.x, platform.y - 2) + offset;

    // Get Object
    currentObjectType = remainingObjectTypes.front();
    remainingObjectTypes.pop();
    switch (currentObjectType)
    {
    case ObjectType::Gem:
        currentObject = new Gem(tileset, pos, texProgram);
        break;
    case ObjectType::Clock:
        currentObject = new Clock(tileset, pos, texProgram);
        break;
    case ObjectType::Shield:
        currentObject = new Shield(tileset, pos, texProgram);
        break;
    }
}

void Scene::detectCheatKeys()
{
    static bool gPressed = false;

    if (Game::instance().getKey('k'))
        showKey = true;

    if (Game::instance().getKey('g') && !gPressed)
    {
        invulnerability = !invulnerability;
        player->setInvulnerabilityStatus(invulnerability);
    }

    gPressed = Game::instance().getKey('g');
}