#ifndef BLOCK_H
#define BLOCK_H

#include "Sprite.h"
#include "Enemy.h"
#include "FlyPathBehaviour.h"

class Block : public GameObject
{
public:

    const static int ONE_WIDE_BLOCKS = 0;
    const static int THREE_WIDE_BLOCKS = 1;

    Block(int type, glm::vec2 p, float speed, int width, std::vector<glm::vec2>& path);
    FlyPathBehaviour flyer;

    glm::vec2 startPosition;

    float speed = 2.0f;

    void update(float delta);

    void reset();
    void init(glm::vec2 p);

    ~Block();
};

#endif // BLOCK_H
