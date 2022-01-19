#ifndef TARGET_H
#define TARGET_H

#include "Sprite.h"
#include "Pickup.h"
#include "FlyPathBehaviour.h"

class Target
{
public:
    Target();
    Target(glm::vec2 p, Pickup* pickup, std::vector<glm::vec2>& path);

    Sprite sprite;
    FlyPathBehaviour flyer;

    Pickup *pickup = nullptr;

    float t = 0;

    void update(float delta);
    void draw(SpriteRenderer* Renderer);
    void reset();

    ~Target();

protected:

private:
};

#endif // TARGET_H
