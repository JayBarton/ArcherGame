#include "Block.h"

Block::Block(int type, glm::vec2 p, float speed, int width, std::vector<glm::vec2>& path)
{
    startPosition = p;
    spawnArea.p = startPosition;

    sprite.setHitBox(glm::vec4(0, 0, width, 32));
    sprite.setSize(glm::vec2(width, 32));
    addCollider(glm::vec4(0, 0, width, 32), Collider::BLOCK);

    flyer = FlyPathBehaviour(speed, path);

    init(p);

    health = Health(1);

    despawnArea = glm::vec2(800, 800);
}

void Block::update(float delta)
{
    flyer.update(delta, sprite);
}

void Block::reset()
{
    flyer.reset();
    init(startPosition);
}

void Block::init(glm::vec2 p)
{
    sprite.active = true;
    sprite.SetPosition(p);
}

Block::~Block()
{
}
