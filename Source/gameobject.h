#pragma once
#include <glm/glm.hpp>
#include <SDL3/SDL.h>
#include <vector>

#include "animation.h"

// This struct defines all objects within the game, including player, background, enemies...

enum class ObjectType
{
	player,
	level,
	enemy,
};

struct GameObject
{
	ObjectType type;
	glm::vec2 position, velocity, acceleration;
	// This dictates if the object is going left or right going from either 1 or -1
	float direction;
	std::vector<Animation> animations;
	int currentAnimation;
	SDL_Texture* texture;

	GameObject() {
		type = ObjectType::level;
		direction = 1;
		position = velocity = acceleration = glm::vec2(0);
		// -1 is to signify no animation at initialisation
		currentAnimation = -1;
		texture = nullptr;
	}
};