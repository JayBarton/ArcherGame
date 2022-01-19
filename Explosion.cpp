#include "Explosion.h"

Explosion::Explosion(glm::vec2 p, glm::vec2 s, const glm::ivec4 & hb,  std::vector<glm::vec4> & u)
{
    sprite.init(p, s, hb, u);
    sprite.startingFrame = 0;
    sprite.active = true;
}

void Explosion::update(float delta)
{
   // bool finished = sprite.playAnimationOnce(delta, 4, true);
    bool finished = sprite.playAnimationOnce(delta, 4, false);
    if(finished)
    {
        sprite.active = false;
    }
}


Explosion::~Explosion()
{
    //dtor
}
