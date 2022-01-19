#include "Target.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"
Target::Target()
{

}


Target::Target(glm::vec2 p, Pickup* pickup, std::vector<glm::vec2>& path)
{
    sprite.SetPosition(p);
    sprite.setHitBox(glm::vec4(0, 0, 32, 32));
    sprite.active = true;
    sprite.currentFrame = 0;

    this->pickup = pickup;
    this->pickup->sprite.alpha = 0;

    flyer = FlyPathBehaviour(2.0f, path);
}

void Target::update(float delta)
{
    flyer.update(delta, sprite);

    pickup->sprite.alpha = glm::mix(0.0f, 1.0f, pow(sin(t), 2));
    t+= 2 * delta;
}

void Target::draw(SpriteRenderer* Renderer)
{
    ResourceManager::GetShader("sprite").Use();

    sprite.currentFrame = 0;
    glm::vec2 position = sprite.getPosition();
    Texture2D texture = ResourceManager::GetTexture("sprites");
    Renderer->setUVs(sprite.getUV());


    Renderer->DrawSprite(texture, position, 0.0f, glm::vec2(32, 32)/*sprite.getSize()*/);

    glm::vec3 color = pickup->sprite.color;
    glm::vec4 colorAndAlpha = glm::vec4(color.x, color.y, color.z, pickup->sprite.alpha);

    sprite.currentFrame = pickup->type + 1;
    Renderer->setUVs(sprite.getUV());
    Renderer->DrawSprite(texture, position, 0.0f, glm::vec2(32, 32), colorAndAlpha);
}

void Target::reset()
{
    flyer.reset();
    sprite.SetPosition(flyer.path[0]);
    t = 0;
}

Target::~Target()
{
    delete pickup;
}
