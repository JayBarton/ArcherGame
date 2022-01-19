#include "PlayerStates.h"

#include <iostream>
PlayerState* PlayerState::handleInput(Player& player, InputManager* inputManager)
{
	//player.direction = 0;

	if (inputManager->isKeyDown(SDLK_LEFT))
	{
	//	player.direction = -1;
		player.facing = -1;
	}
	else if (inputManager->isKeyDown(SDLK_RIGHT))
	{
	//	player.direction = 1;
		player.facing = 1;
	}

	return nullptr;
}

PlayerState * PlayerState::update(Player & player)
{
	return nullptr;
}

PlayerState * PlayerState::checkBounds(Player & player)
{
	
	return nullptr;
} 

PlayerState* StandingState::handleInput(Player& player, InputManager* inputManager)
{
	PlayerState::handleInput(player, inputManager);
	if (!inputManager->isKeyDown(SDLK_q) && inputManager->isKeyPressed(SDLK_DOWN))
	{
		return new DuckingState();
	}
	if (inputManager->isKeyPressed(SDLK_w))
	{
		player.moveSpeed.y = -9.0f;
		return new AirState();
	}
	//If the player is already holding up when they go to aim, aim straight up
	if (inputManager->isKeyPressed(SDLK_q))
	{
		if (inputManager->isKeyDown(SDLK_UP))
		{
			player.rotationDirection = UP_ROTATION;
		}
	}

	if (inputManager->isKeyDown(SDLK_q))
	{
		dash = false;
		player.moveSpeed.x = 2.5f;
		if (inputManager->isKeyDown(SDLK_UP))
		{
			player.rotationDirection -= 5.0f;
			if (player.rotationDirection < UP_ROTATION)
			{
				player.rotationDirection = UP_ROTATION;
			}
		}
		else if (inputManager->isKeyDown(SDLK_DOWN))
		{
			player.rotationDirection += 5.0f;
			if (player.rotationDirection > DOWN_ROTATION)
			{
				player.rotationDirection = DOWN_ROTATION;
			}
		}
	}

	//dash
	if (inputManager->keyDoublePress(SDLK_RIGHT) || inputManager->keyDoublePress(SDLK_LEFT))
	{
		player.moveSpeed.x = 5.0f;
		dash = true;
	}

	//fire
	if (inputManager->isKeyReleased(SDLK_q))
	{
		player.moveSpeed.x = 5.0f;
		player.rotationDirection = 0;
	}

	return nullptr;
}

void StandingState::enterState(Player& player)
{
	player.moveSpeed.x = 5.0f;
	player.sprite.setHitBox(glm::vec4(0, 0, 32, 32));
}

PlayerState* StandingState::update(Player&player)
{
	if (dash)
	{
		player.moveSpeed.x += 1.0f;
		if (player.moveSpeed.x >= 9.0f)
		{
			player.moveSpeed.x = 9.0f;
		}
	}
	//if (player.direction == 0)
	{
		dash = false;
		player.moveSpeed.x = 5.0f;
	}
	return nullptr;
}

PlayerState* DuckingState::handleInput(Player& player, InputManager* inputManager)
{
	PlayerState::handleInput(player, inputManager);
	if (inputManager->isKeyDown(SDLK_q))
	{
		//charge shot
	}
	if (inputManager->isKeyReleased(SDLK_q))
	{
		player.rotationDirection = 0;
	}

	if (inputManager->isKeyReleased(SDLK_DOWN))
	{
		return new StandingState();
	}
	return nullptr;
}

void DuckingState::enterState(Player& player)
{
	player.moveSpeed.x = 2.5f;
	player.sprite.setHitBox(glm::vec4(0, 16, 32, 16));
}

PlayerState* DuckingState::update(Player& player)
{
	return nullptr;
}

PlayerState * AirState::handleInput(Player & player, InputManager * inputManager)
{
	if (inputManager->isKeyDown(SDLK_LEFT))
	{
		if (player.facing > 0)
		{
			player.moveSpeed.x = 0.0f;
			player.facing = -1;
		//	player.direction = -1;
		}
		player.moveSpeed.x += 0.1f;
		if (player.moveSpeed.x >= 5.0f)
		{
			player.moveSpeed.x = 5.0f;
		}

	}
	else if (inputManager->isKeyDown(SDLK_RIGHT))
	{
		if (player.facing < 0)
		{
			player.moveSpeed.x = 0.0f;
			player.facing = 1;
		//	player.direction = 1;
		}
		player.moveSpeed.x += 0.1f;
		if (player.moveSpeed.x >= 5.0f)
		{
			player.moveSpeed.x = 5.0f;
		}
	}
	return nullptr;
}

PlayerState* AirState::update(Player & player)
{

	if (player.moveSpeed.x > 0 && !player.inputManager->isKeyDown(SDLK_LEFT) && !player.inputManager->isKeyDown(SDLK_RIGHT))
	{
		player.moveSpeed.x -= 0.4f;
	}
	if (player.moveSpeed.x < 0)
	{
		player.moveSpeed.x = 0;
	}

	if (player.moveSpeed.y < maxFallSpeed)
	{
		player.moveSpeed.y += fallAcceleration;
	}
	else
	{
		player.moveSpeed.y = maxFallSpeed;
	}

	//This should be done later
	glm::vec2 position = player.sprite.getPosition();
	if (position.y > 200)
	{
		player.moveSpeed.y = 0;
		player.sprite.SetPosition(glm::vec2(position.x, 200));
		return new StandingState();
	}
	//std::cout << position.y << std::endl;;
	return nullptr;
}

PlayerState * AirState::checkBounds(Player & player)
{
	return nullptr;
}

void AirState::enterState(Player & player)
{

}
