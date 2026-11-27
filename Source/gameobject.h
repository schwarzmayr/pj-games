#pragma once
#include <glm/glm.hpp>
#include <SDL3/SDL.h>
#include <vector>

#include "animation.h"

// Create a enum that handles the different states of a player, this enum can be copied to track other objects throughout the game
enum class PlayerState
{
	idle,
	running,
	jumping,
};

struct PlayerData
{
	PlayerState state;
	PlayerData() {
		state = PlayerState::idle;
	}
};

struct LevelData {};
struct EnemyData {};

// Unions are ways of handling memory as all data types in the union are assigned the same space of memory
// Therefore when updated, it will only be holding the info of the data type MOST RECENTLY called
// This allows us to check the type first and assign the appropriate data struct to the object
union ObjectData
{
	PlayerData player;
	LevelData level;
	EnemyData enemy;
};

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
	ObjectData data;
	glm::vec2 position, velocity, acceleration;
	// This dictates if the object is going left or right going from either 1 or -1
	float direction;
	float maxSpeedX;
	std::vector<Animation> animations;
	int currentAnimation;
	SDL_Texture* texture;
	// Bool to determine if object is a static non-moving object or can walk
	bool dynamic;
	bool grounded;
	// The collider is used to reflect the actual sprite size inside the 
	SDL_FRect collider;

	// This initiates the data to level via initialising instantly with values instead of initialising it empty then adding data
	// via the body
	GameObject() : data{.level = LevelData()}, collider(0) {
		type = ObjectType::level;
		direction = 1;
		position = velocity = acceleration = glm::vec2(0);
		// -1 is to signify no animation at initialisation
		currentAnimation = -1;
		texture = nullptr;
		dynamic = false;
		grounded = false;
	}
};