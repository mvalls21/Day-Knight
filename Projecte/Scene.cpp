#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include "Scene.h"
#include "Game.h"
#include "ShaderSystem.h"
#include "SoundManager.h"

#include <random>

#define SCREEN_X 26
#define SCREEN_Y 54

#define INIT_PLAYER_X_TILES 8
#define INIT_PLAYER_Y_TILES 3

#define TILE_POS(x, y) glm::vec2(SCREEN_X + (x)*map->getTileSize(), SCREEN_Y + (y)*map->getTileSize())

// the following defines tweak everything related to the conversion from remaining time to extra points when the lvl is passed
#define LVL_TRANSITION_TIME 3500
#define SCORE_UP_LVL_PASSED_PERIOD 50
#define SCORE_UP_LVL_PASSED_END 1000
#define SCORE_UP_LVL_PASSED_START LVL_TRANSITION_TIME - 1500
#define EXTRA_POINTS_FOR_SECOND 20

Scene::Scene()
{
    map = NULL;
    player = NULL;

    text = new Text();
    if (!text->init("fonts//OldLondon.ttf"))
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
    player = new Player(description.lives);
    player->init(glm::ivec2(SCREEN_X, SCREEN_Y), *texProgram);
    player->setPosition(glm::vec2(description.playerPositionStartTile.x * map->getTileSize(), description.playerPositionStartTile.y * map->getTileSize() - 4));
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

    topBarTex = new Texture();
    topBarTex->loadFromFile("images/top_bar.png", PixelFormat::TEXTURE_PIXEL_FORMAT_RGBA);
    topBar = StaticSprite::createSprite(glm::vec2(16.0f * 32, 16.0f * 3), glm::vec2(1.0f), topBarTex, texProgram);
    topBar->setSpritesheetCoords(glm::vec2(0.0f));
    topBar->setPosition(glm::vec2(SCREEN_X, SCREEN_Y - 16.0f * 3));

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
    lives = description.lives;

    SoundManager::getManager().playSoundtrack("sounds/gameplaySoundtrack.wav");

    // First update to position everything in its place
    player->update(0);
    for (const auto &enemy : enemies)
        enemy->update(0);
}

SceneStatus Scene::update(int deltaTime)
{
    currentTime += deltaTime;

    if (currentTime < TIME_TO_START)
    {
        return SceneStatus::Continue;
    }
    else if (timeUpRemainingTimeTransition > 0)
    {
        timeUpRemainingTimeTransition -= deltaTime;
        if (timeUpRemainingTimeTransition <= 0)
            return SceneStatus::TimeIsUp;
        return SceneStatus::Continue;
    }
    else if (levelPassedRemainingTimeTransition > 0)
    {
        if (levelPassedRemainingTimeTransition >= SCORE_UP_LVL_PASSED_END and levelPassedRemainingTimeTransition <= SCORE_UP_LVL_PASSED_START)
        {
            int remainingScoreUpTicks = (levelPassedRemainingTimeTransition - SCORE_UP_LVL_PASSED_END) / SCORE_UP_LVL_PASSED_PERIOD;
            if (remainingScoreUpTicks != (levelPassedRemainingTimeTransition - deltaTime - SCORE_UP_LVL_PASSED_END) / SCORE_UP_LVL_PASSED_PERIOD)
            {
                SoundManager::getManager().playStackableSound("sounds/scoreUp.wav");
                if (remainingScoreUpTicks == 1)
                {
                    *score += (levelTimer / 100) * (EXTRA_POINTS_FOR_SECOND / 10);
                    levelTimer = 0;
                    SoundManager::getManager().playSound("sounds/timeToPointsEnd.wav");
                }
                else
                {
                    *score += ((levelTimer / 100) / remainingScoreUpTicks) * (EXTRA_POINTS_FOR_SECOND / 10);
                    levelTimer -= ((levelTimer / 100) / remainingScoreUpTicks) * 100;
                }
            }
        }

        levelPassedRemainingTimeTransition -= deltaTime;
        if (levelPassedRemainingTimeTransition > 0)
        {
            return SceneStatus::Continue;
        }
        else
        {
            return SceneStatus::LevelComplete;
        }
    }

    if (hasStopwatch)
    {
        timeSinceLastStopwatch_ms += deltaTime;

        if ((totalStopwatchTime_ms - timeSinceLastStopwatch_ms) <= 2000 && (int(currentTime / 200) % 2) == 0)
        {
            for (const auto& enemy : enemies)
                enemy->toggleDark();
        }

        if (timeSinceLastStopwatch_ms >= totalStopwatchTime_ms)
        {
            for (const auto& enemy : enemies)
                enemy->disableDark();

            hasStopwatch = false;
        }
    }

    if (!hasStopwatch)
        levelTimer -= deltaTime;

    if (levelTimer <= 0)
    {
        SoundManager::getManager().stopAllSounds();
        SoundManager::getManager().playSound("sounds/timeUp.wav");
        timeUpRemainingTimeTransition = 4000;
    }
    else if (levelTimer / 1000 < 10 and levelTimer / 1000 != (levelTimer + deltaTime) / 1000)
    {
        SoundManager::getManager().playStackableSound("sounds/timeLow.wav");
    }

    detectCheatKeys();
    objectTimer += deltaTime;

    map->update(deltaTime);

    player->update(deltaTime);

    if (!hasStopwatch)
    {
        for (auto *enemy : enemies)
            enemy->update(deltaTime);
    }

    bool enemyCollision = !player->isImmune() && std::any_of(enemies.begin(), enemies.end(), [&](const Enemy *enemy)
                                                             { return player->isColliding(*enemy); });

    if (enemyCollision && !invulnerability)
    {
        if (shieldProtection)
            shieldProtection = false;
        else
        {
            player->setLives(player->getLives() - 1);
        }

        if (player->getLives() != 0)
        {
            player->makeImmune(PLAYER_IMMUNITY_MS);
        }
    }

    if (player->getLives() == 0 && player->finishedDeath())
    {
        return SceneStatus::PlayerDead;
    }
    else if (player->getLives() == 0)
    {
        SoundManager::getManager().playSound("sounds/gameOver.wav");
        return SceneStatus::Continue;
    }

    // Object stuff (key, door, powerups...)

    if (!showKey && !isDoorOpen && map->isCompleted())
        showKey = true;

    if (showKey && !isDoorOpen)
    {
        if (player->isColliding(*key))
        {
            SoundManager::getManager().playSound("sounds/doorOpen.wav");
            isDoorOpen = true;
            showKey = false;
            door->open();
        }
    }

    door->update(deltaTime);

    if (isDoorOpen && player->isColliding(*door))
    {
        SoundManager::getManager().stopAllSounds();
        SoundManager::getManager().playSound("sounds/win.wav");
        levelPassedRemainingTimeTransition = LVL_TRANSITION_TIME;
    }

    if (currentObjectType != ObjectType::None && currentObject->isColliding(*player))
    {
        SoundManager::getManager().playSound("sounds/objectCollect.wav");
        switch (currentObjectType)
        {
        case ObjectType::Gem:
            *score += GEM_SCORE_BONUS;
            break;
        case ObjectType::Clock:
            hasStopwatch = true;
            timeSinceLastStopwatch_ms = 0;
            break;
        case ObjectType::Shield:
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
    topBar->render();
    door->render();

    for (int i = 0; i < player->getLives(); ++i)
    {
        if (i == player->getLives() - 1 && shieldProtection)
        {
            shield->setPosition(glm::vec2(SCREEN_X + 25 + 20 * i, SCREEN_Y - 24));
            shield->render();
            continue;
        }

        heart->setPosition(glm::vec2(SCREEN_X + 25 + 20 * i, SCREEN_Y - 25));
        heart->render();
    }

    for (const auto *enemy : enemies)
        enemy->render();

    if (showKey)
        key->render();

    if (currentObjectType != ObjectType::None)
        currentObject->render();

    player->render();

    // TEXT

    int dimensions[4];
    glGetIntegerv(GL_VIEWPORT, dimensions);
    glm::vec2 pos;

    constexpr float size = 70.0f;

    float middleX = (float)dimensions[2] / 2.0f;
    float middleY = (float)dimensions[3] / 2.0f;

    if (levelPassedRemainingTimeTransition > 0)
    {
        const std::string content("Stage Cleared!");
        text->render(content, glm::vec2(middleX - 176.0f, middleY), size, glm::vec4(1.0f, 1.0f, 1.0f, 0.8f));
    }
    else if (currentTime < TIME_TO_START)
    {
        const std::string content("Ready?");
        text->render(content, glm::vec2(middleX - 84.0f, middleY), size, glm::vec4(1.0f, 1.0f, 1.0f, 0.8f));
    }

    glm::vec4 color = levelPassedRemainingTimeTransition > 0 ? glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) : levelTimer >= 10000 ? glm::vec4(1.0f)
                                                                                                                       : glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    pos = glm::vec2(655.0f * dimensions[2] / SCREEN_WIDTH, 110.0f * dimensions[3] / SCREEN_HEIGHT);

    texProgram->use();

    if (hasStopwatch && (totalStopwatchTime_ms  - timeSinceLastStopwatch_ms) <= 2000 && (int(currentTime / 200)  % 2) == 0) 
        color = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);

    text->render(std::to_string(((levelTimer + 999) / 1000) / 10) + std::to_string(((levelTimer + 999) / 1000) % 10), pos, 60.0f, color);

    string stageNumString = string(1, '0' + (stageNum / 10) % 10) + string(1, '0' + (stageNum % 10));
    pos = glm::vec2(1075.0f * dimensions[2] / SCREEN_WIDTH, 110.0f * dimensions[3] / SCREEN_HEIGHT);
    text->render("Stage " + stageNumString, pos, 60.0f, glm::vec4(1.0f));

    string scoreString = string(1, '0' + (*score / 10000) % 10) + string(1, '0' + (*score / 1000) % 10) +
                         string(1, '0' + (*score / 100) % 10) + string(1, '0' + (*score / 10) % 10) + string(1, '0' + (*score) % 10);
    pos = glm::vec2(350.0f * dimensions[2] / SCREEN_WIDTH, 110.0f * dimensions[3] / SCREEN_HEIGHT);
    text->render(scoreString, pos, 60.0f, glm::vec4(1.0f));
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

    if (Game::instance().getKey('k') && !isDoorOpen)
        showKey = true;

    if (Game::instance().getKey('g') && !gPressed)
    {
        invulnerability = !invulnerability;
        player->setInvulnerabilityStatus(invulnerability);
    }

    gPressed = Game::instance().getKey('g');
}