// Based off constred youtube tutorial: https://www.youtube.com/watch?v=Wu2g-N5Z78Y

#include "demo.h"

int main(int argc, char *argv[]){

	SDLState state{};
	state.width = 800;
	state.height = 600;
	state.logW = 640;
	state.logH = 320;

	if (!initialize(state)) {
		return 1;
	}

	// load game assets
	Resources res;
	res.load(state);

	// Create the game state holding all layers
	GameState gs;

	// create player object
	GameObject player;
	player.type = ObjectType::player;
	player.texture = res.texIdle;
	player.animations = res.playerAnims;
	player.currentAnimation = res.ANIM_PLAYER_IDLE;
	gs.layers[LAYER_IDX_CHARACTER].push_back(player);

	// setup game data
	const bool* keys = SDL_GetKeyboardState(nullptr);
	uint64_t prevTime = SDL_GetTicks();

	// start game loop
	bool running = true;
	while (running) {

		// gets the current time elapsed since initialisation
		uint64_t nowTime = SDL_GetTicks();
		// deltatime is divided by 1000 since GetTicks return in millisecond and we want our time in seconds
		float deltaTime = (nowTime - prevTime) / 1000.0f;

		// standard variable to handle events in SDL
		SDL_Event event{0};
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_EVENT_QUIT:
					running = false;
					break;
				case SDL_EVENT_WINDOW_RESIZED:
					state.width = event.window.data1;
					state.height = event.window.data2;
					break;
			}
		}

/*		// handle movement, in games, we do this by coding the amount per second rather than just an arbritary number
		float moveAmount = 0;
		if (keys[SDL_SCANCODE_A]) {
			moveAmount += -75.0f;
			flipHorizontal = true;
		} 
		if (keys[SDL_SCANCODE_D]) {
			moveAmount += 75.0f;
			flipHorizontal = false;
		}
		playerX += moveAmount * deltaTime; */


		// The below code all the way to draw all objects is what ties the animation loop to the game
		// update all objects before they are drawn
		for (auto& layer : gs.layers) {
			for (GameObject& obj : layer) {
				if (obj.currentAnimation != -1) {
					obj.animations[obj.currentAnimation].step(deltaTime);
				}
			}
		}

		// perform drawing commands
		SDL_SetRenderDrawColor(state.renderer, 20, 10, 30, 255);
		SDL_RenderClear(state.renderer);

		// draw all objects
		for (auto& layer : gs.layers) {
			for (GameObject& obj : layer) {
				drawObject(state, gs, obj, deltaTime);
			}
		}

		// swap buffer and present
		SDL_RenderPresent(state.renderer);
		prevTime = nowTime;
	}

	res.unload();
	cleanup(state);
	return 0;
}

bool initialize(SDLState &state) {

	bool initSuccess = true;

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing SDL3", nullptr);
		initSuccess = false;
	}

	// create the window
	state.window = SDL_CreateWindow("SDL3 Demo", state.width, state.height, SDL_WINDOW_RESIZABLE);
	if (!state.window) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing window", nullptr);
		cleanup(state);
		initSuccess = false;
	}

	// create the renderer
	state.renderer = SDL_CreateRenderer(state.window, NULL);
	if (!state.renderer) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing renderer", state.window);
		cleanup(state);
		initSuccess = false;
	}

	// configure presentation allows SDL to determine how to scale the image properly regardless of windows size following the logical presentation
	// meaning even if resizing the window, SDL will determine how best to resizing all assets
	// Letterbox introduces blackbars as needed if scale is not following the aspect ratio noted
	SDL_SetRenderLogicalPresentation(state.renderer, state.logW, state.logH, SDL_LOGICAL_PRESENTATION_LETTERBOX);
	return initSuccess;
}

void cleanup(SDLState &state) {

	SDL_DestroyRenderer(state.renderer);
	SDL_DestroyWindow(state.window);
	SDL_Quit();
}

void drawObject(const SDLState& state, GameState& gs, GameObject& obj, float deltaTime) {
	// Set a constant variable to be used for logical size to keep scaling constant
	const float spriteSize = 32;
	float srcX = obj.currentAnimation != -1 ? obj.animations[obj.currentAnimation].currentFrame() * spriteSize : 0.0f;
	// Create a variable with SDL_FRect to specify part of the image to draw
	SDL_FRect src{
		.x = srcX,
		.y = 0,
		.w = spriteSize,
		.h = spriteSize,
	};

	SDL_FRect dst{
		.x = obj.position.x,
		.y = obj.position.y,
		.w = spriteSize,
		.h = spriteSize,
	};

	SDL_FlipMode flipmode = obj.direction == -1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
	// Places the sprite and flips it depending on if the user is pressing the a or d key
	SDL_RenderTextureRotated(state.renderer, obj.texture, &src, &dst, 0, nullptr, flipmode);
}