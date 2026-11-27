#pragma once

#include <array>
#include <format>
#include <glm/glm.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <vector>

// As gameobject.h alrdy includes animation.h, we can use that instead
#include "gameobject.h"

using namespace std;

struct SDLState
{
	SDL_Window *window;
	SDL_Renderer *renderer;
	int width, height, logW, logH;
	const bool* keys;

	SDLState() : keys(SDL_GetKeyboardState(nullptr)) {}
};

// This will hold almost everything on screen i.e. game objects, players
const size_t LAYER_IDX_LEVEL = 0;
const size_t LAYER_IDX_CHARACTERS = 1;
const int MAP_ROWS = 5;
const int MAP_COLS = 50;
const int TILE_SIZE = 32;

struct GameState
{
	std::array<std::vector<GameObject>, 2> layers;
	// Use to capture the player index within the game object
	int playerIndex;
	SDL_FRect mapViewport;

	GameState(const SDLState &state)
	{
		playerIndex = -1;
		mapViewport = SDL_FRect{
			.x = 0,
			.y = 0,
			.w = static_cast<float>(state.logW),
			.h = static_cast<float>(state.logH),
		};
	}
	// Function to get player object when loaded
	GameObject& player() { return layers[LAYER_IDX_CHARACTERS][playerIndex]; }
};

// Creating a struct to load/unload assets and pass around functions is the simplest way possible for managing them
struct Resources
{
	// 0 represents the index of the idle animation that will down the line index into the list of idle animations
	const int ANIM_PLAYER_IDLE = 0;
	const int ANIM_PLAYER_RUN = 1;
	const int ANIM_PLAYER_SLIDE = 2;
	std::vector<Animation> playerAnims{};

	// This one is to hold SDL_Texture pointers
	std::vector<SDL_Texture*> textures;
	// Pointer to idle texture
	SDL_Texture *texIdle, *texRun, *texSlide, *texBrick, *texGrass, *texGround, *texPanel;

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
		playerAnims[ANIM_PLAYER_RUN] = Animation(4, 0.5f);
		playerAnims[ANIM_PLAYER_SLIDE] = Animation(1, 1.0f);

		// Remember that these are all pointers that point to the IMG_LoadTexture in the function created above
		texIdle = loadTexture(state.renderer, "data/idle.png");
		texRun = loadTexture(state.renderer, "data/run.png");
		texSlide = loadTexture(state.renderer, "data/slide.png");
		texBrick = loadTexture(state.renderer, "data/tiles/brick.png");
		texGrass = loadTexture(state.renderer, "data/tiles/grass.png");
		texGround = loadTexture(state.renderer, "data/tiles/ground.png");
		texPanel = loadTexture(state.renderer, "data/tiles/panel.png");
	}

	void unload() {
		for (SDL_Texture* tex : textures) {
			SDL_DestroyTexture(tex);
		}
	}
};

bool initialize(SDLState&);
void cleanup(SDLState&);
void drawObject(const SDLState&, GameState&, GameObject&, float);
void update(const SDLState&, GameState&, Resources&, GameObject&, float);
void createTiles(const SDLState&, GameState&, const Resources&);
void collisionResponse(const SDLState&, GameState&, const Resources&, const SDL_FRect&, const SDL_FRect&, const SDL_FRect&, GameObject&, GameObject&, float);
void checkCollision(const SDLState&, GameState&, const Resources&, GameObject&, GameObject&, float);
void handleKeyInput(const SDLState&, GameState&, GameObject&, SDL_Scancode, bool);