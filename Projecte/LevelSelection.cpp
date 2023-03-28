#include "LevelSelection.h"

constexpr float relation = 1.36;

static Scene *sceneLevel01()
{
    Scene *scene = new Scene();

    Scene::Description description{};

    description.levelName = "level01.txt";
    description.playerPositionStartTile = {8, 3};

    description.skeletonDescriptions = {
        Scene::EnemyDescription{22, 18, MOVE_RIGHT},
        Scene::EnemyDescription{8, 18, MOVE_LEFT}};

    description.vampireDescriptions = {
        Scene::EnemyDescription{6, 9, MOVE_RIGHT},
        Scene::EnemyDescription{24, 9, MOVE_LEFT}};

    description.keyPositionTile = {8, 18};
    description.doorPositionTile = {21, 4};

    scene->init(description);

    return scene;
}

LevelSelection::LevelSelection()
{
    text = new Text();
    if (!text->init("/home/david/Downloads/OpenGL_VS2015/OpenGL/06-text/06-text/fonts/OpenSans-Regular.ttf"))
        exit(0);

    auto *level01 = sceneLevel01();
    auto *level02 = sceneLevel01();

    for (int i = 0; i < 3; ++i)
    {
        auto *level = sceneLevel01();
        level->update(0);
        levels.push_back(level);
    }
}

LevelSelection::~LevelSelection()
{
    for (auto level : levels)
    {
        delete level;
    }
}

void LevelSelection::render() const
{
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    const unsigned int width = vp[2];
    const unsigned int height = vp[3];

    const float heightPerSlot = levels.size() <= 3
                                    ? (float)height / 3.0f
                                    : (float)height / 4.0f;

    {
        glViewport(0, 0, width, heightPerSlot);

        const int pixelSize = 50;
        std::string backStr("Back");
        text->render(backStr, glm::vec2(width / 2.0f - pixelSize, heightPerSlot - pixelSize), pixelSize, glm::vec4(1.0f));
    }

    {
        float offsetLeft = 0.05f * width;

        float percentageWidthPerSlot = (1.0f - 0.05f * 2.0f) / std::min((int)levels.size(), 3);
        float slotWidth = percentageWidthPerSlot * width;

        float height = std::min(slotWidth / 1.36f, heightPerSlot);

        for (int i = 0; i < levels.size(); ++i)
        {
            float startHeight = i >= 3 ? 2.0f * heightPerSlot : heightPerSlot;
            float offsetOffset = (i % 3) * slotWidth;

            glViewport(offsetLeft + offsetOffset, startHeight, slotWidth, height);
            levels[i]->render();
        }
    }

    {
        glViewport(0.0f, heightPerSlot * 2.0f, width, heightPerSlot);

        const int pixelSize = 50;
        std::string str("Level Selection");
        const unsigned int numLetters = str.length();

        text->render(str, glm::vec2(width / 2, heightPerSlot - pixelSize * 2.0f), pixelSize, glm::vec4(1.0f));
    }

    // Restore initial vieport
    glViewport(0, 0, width, height);
}
