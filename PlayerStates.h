#pragma once
#include "Player.h"

class Player;

class PlayerState
{
public:

	virtual ~PlayerState() {}
	virtual PlayerState* handleInput(Player& player, InputManager* inputManager);
	virtual PlayerState* update(Player& player);
	virtual PlayerState* checkBounds(Player& player);
	virtual void enterState(Player& player) {}
};



class StandingState : public PlayerState
{
private:
	const static int UP_ROTATION = -90;
	const static int DOWN_ROTATION = 45;

	float standingMoveSpeed = 5.0f;

	bool dash = false;
public:
	StandingState() {}

	PlayerState* handleInput(Player& player, InputManager* inputManager);

	PlayerState* update(Player& player);

	void enterState(Player& player);

};

class DuckingState : public PlayerState
{
public:
	DuckingState() {}

	PlayerState* handleInput(Player& player, InputManager* inputManager);

	PlayerState* update(Player& player);

	void enterState(Player& player);

};

class AirState : public PlayerState
{
private:

	float maxAirSpeed;

	float maxFallSpeed = 9.0f;

	float fallAcceleration = 1.2f;

public:
	AirState() {}

	PlayerState* handleInput(Player& player, InputManager* inputManager);

	PlayerState* update(Player& player);

	PlayerState* checkBounds(Player& player);

	void enterState(Player& player);

};