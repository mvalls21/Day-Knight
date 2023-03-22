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
        float widthPerSlot = (float)width / 3.0f;
        const float localHeight = std::min((float)widthPerSlot / relation, heightPerSlot);

        if (levels.size() <= 3)
        {
            if (localHeight == heightPerSlot)
            {
                widthPerSlot = localHeight * relation;
            }

            if (levels.size() == 1)
            {
                glViewport(widthPerSlot, heightPerSlot, widthPerSlot, localHeight);
                levels[0]->render();
            }
            else if (levels.size() == 2)
            {
                const float offset = 100.0f;
                glViewport(offset, heightPerSlot, widthPerSlot, localHeight);
                levels[0]->render();

                glViewport(widthPerSlot * 2.0f - offset, heightPerSlot, widthPerSlot, localHeight);
                levels[1]->render();
            }
            else
            {
                glViewport(0.0f, heightPerSlot, widthPerSlot, localHeight);
                levels[0]->render();

                glViewport(widthPerSlot, heightPerSlot, widthPerSlot, localHeight);
                levels[1]->render();

                glViewport(widthPerSlot * 2.0f, heightPerSlot, widthPerSlot, localHeight);
                levels[2]->render();
            }
        }
        else
        {
            for (int i = 0; i < levels.size(); ++i)
            {
                glViewport(widthPerSlot * i, heightPerSlot * 2.0f, widthPerSlot, localHeight);
                levels[i]->render();
            }

            for (int i = 3; i < levels.size(); ++i)
            {
                glViewport(widthPerSlot * (i - 3), heightPerSlot, widthPerSlot, localHeight);
                levels[i]->render();
            }
        }

        {
        glViewport(0.0f, heightPerSlot * 3.0f, width, heightPerSlot);

        const int pixelSize = 50;
        std::string backStr("Level Selection");
        text->render(backStr, glm::vec2(width / 2.0f - pixelSize, heightPerSlot - pixelSize *2.0f), pixelSize, glm::vec4(1.0f));

        }

        // // Levels on first line
        // glViewport(widthPerSlot, heightPerSlot, widthPerSlot, heightPerSlot);
        // levels[0]->render();

        // glViewport(widthPerSlot * 2.0f, heightPerSlot, widthPerSlot, heightPerSlot);
        // levels[1]->render();
    }

    // Restore initial vieport
    glViewport(0, 0, width, height);
}
