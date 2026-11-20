#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <vector>

#include "animation.h"

struct SDLState
{
	SDL_Window *window;
	SDL_Renderer *renderer;
	int width, height, logW, logH;
};

using namespace std;

bool initialize(SDLState &state);
void cleanup(SDLState &state);

// Creating a struct to load/unload assets and pass around functions is the simplest way possible for managing them
struct Resources
{
	// 0 represents the index of the idle animation that will down the line index into the list of idle animations
	const int ANIM_PLAYER_IDLE = 0;
	std::vector<Animation> playerAnims{};

	// This one is to hold SDL_Texture pointers
	std::vector<SDL_Texture*> textures;
	// Pointer to idle texture
	SDL_Texture* texIdle;

	SDL_Texture* loadTexture(SDL_Renderer* renderer, const string &filepath) {
		// load game assets. SDL_texture is a memory holding an image on a graphics card
		// A surface object on the other hands allows us to access the memory that is stored in the CPU instead.
		// Since SDL is a C-style api, we need to convert our string to that style from C++ string
		SDL_Texture* tex = IMG_LoadTexture(renderer, filepath.c_str());
		SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
		textures.push_back(tex);
		return tex;
	}

	void load(SDLState& state) {
		// This lets us index into the vector by placing 5 empty indexs (.reserve only allocates memory but doesn't assign it)
		playerAnims.resize(5);
		// This gives the animation a total of 8 frames with 1.6 secs length
		playerAnims[ANIM_PLAYER_IDLE] = Animation(8, 1.6f);

		texIdle = loadTexture(state.renderer, "data/idle.png");
	}

	void unload() {
		for (SDL_Texture* tex : textures) {
			SDL_DestroyTexture(tex);
		}
	}
};