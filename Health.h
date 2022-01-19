#ifndef HEALTH_H
#define HEALTH_H


class Health
{
public:
    Health();
    Health (int maxHealth);

    float hitTimer = 0.0f;
    float hitTime = 0.125f;
    float damageTime = 1.0f;

    //How long the death animation lasts
	float deathTimer = 0.0f;
	float deathTime = 0.4f;

    int currentHealth;

    bool damaged = false; //can't take damage for a bit after being hit
    bool hit = false; //prevents input shortly after being hit (should be part of player
    bool dead = false;
    bool dying = false; //For death animations, want a state between alive and dead

    bool special = false; //For special death effects

    bool takeDamage(int damage);
    void updateDamaged(float delta);
    void updateDeath(float delta);
    bool alive();
    void fullHeal();

    ~Health();
};

#endif // HEALTH_H
