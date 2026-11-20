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

	// load game assets. SDL_texture is a memory holding an image on a graphics card
	// A surface object on the other hands allows us to access the memory that is stored in the CPU instead.
	SDL_Texture *idleTex = IMG_LoadTexture(state.renderer, "data/idle.png");
	SDL_SetTextureScaleMode(idleTex, SDL_SCALEMODE_NEAREST);

	// setup game data
	const bool *keys = SDL_GetKeyboardState(nullptr);
	float playerX = 150;
	const float floor = state.logH;
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

		// handle movement, in games, we do this by coding the amount per second rather than just an arbritary number
		float moveAmount = 0;
		if (keys[SDL_SCANCODE_A]) {
			moveAmount += -75.0f;
		} 
		if (keys[SDL_SCANCODE_D]) {
			moveAmount += 75.0f;
		}
		playerX += moveAmount * deltaTime;

		// perform drawing commands
		SDL_SetRenderDrawColor(state.renderer, 20, 10, 30, 255);
		SDL_RenderClear(state.renderer);

		// Set a constant variable to be used for logical size to keep scaling constant
		const float spriteSize = 32;
		// Create a variable with SDL_FRect to specify part of the image to draw
		SDL_FRect src{
			.x = 0,
			.y = 0,
			.w = spriteSize,
			.h = spriteSize,
		};

		SDL_FRect dst{
			.x = playerX,
			.y = floor - spriteSize,
			.w = spriteSize,
			.h = spriteSize,
		};

		SDL_RenderTexture(state.renderer, idleTex, &src, &dst);

		// swap buffer and present
		SDL_RenderPresent(state.renderer);
		prevTime = nowTime;
	}

	SDL_DestroyTexture(idleTex);
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