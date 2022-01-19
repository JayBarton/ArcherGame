#ifndef FLYPATHBEHAVIOUR_H
#define FLYPATHBEHAVIOUR_H

#include <glm.hpp>
#include "Sprite.h"

class FlyPathBehaviour
{
public:
    FlyPathBehaviour() {}
    FlyPathBehaviour(float speed, std::vector<glm::vec2>& path);

    void update(float delta, Sprite &sprite);

    void movement(Sprite &sprite);

    void findNextNode();
    void getNewDirection();

    void reset();

    float speed;

    glm::vec2 nextNode;
	glm::vec2 direction;

	std::vector<glm::vec2> path;

	int end;
	int current;

    ~FlyPathBehaviour();

};

#endif // FLYPATHBEHAVIOUR_H
