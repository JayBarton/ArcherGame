#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "Sprite.h"

class Explosion
{
public:
    Explosion(glm::vec2 p, glm::vec2 s, const glm::ivec4 & hb,  std::vector<glm::vec4> & u );

    Sprite sprite;

    void update(float delta);
    ~Explosion();
};

#endif // EXPLOSION_H
