// Based off constred youtube tutorial: https://www.youtube.com/watch?v=Wu2g-N5Z78Y

#include "demo.h"

int main(int argc, char *argv[]){

	SDLState state{};
	state.width = 1200;
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
	GameState gs(state);

	// Uses createTiles function to draw the map following the local array design
	createTiles(state, gs, res);

	// setup game data
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
				// These ones are better for single instance cases, whereas update() is better for continuous motion key pressing instances
				case SDL_EVENT_KEY_DOWN:
					handleKeyInput(state, gs, gs.player(), event.key.scancode, true);
					break;
				case SDL_EVENT_KEY_UP:
					handleKeyInput(state, gs, gs.player(), event.key.scancode, false);
					if (event.key.scancode == SDL_SCANCODE_F11) {
						// Basically allowing toggle of debug mode when to on or off
						gs.debugMode = !gs.debugMode;
					}
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
				// update positions
				update(state, gs, res, obj, deltaTime);
				// update animations
				if (obj.currentAnimation != -1) {
					obj.animations[obj.currentAnimation].step(deltaTime);
				}
			}
		}

		// For loop to update bullet physics and animations
		for (GameObject& bullet : gs.bullets) {
			// update positions
			update(state, gs, res, bullet, deltaTime);
			// update animations
			if (bullet.currentAnimation != -1) {
				bullet.animations[bullet.currentAnimation].step(deltaTime);
			}
		}

		// calculate viewpoint position
		// This is performed before drawing but right after all object updates to give the sense the world is moving around the player
		gs.mapViewport.x = (gs.player().position.x + TILE_SIZE / 2) - (gs.mapViewport.w / 2);

		// perform drawing commands
		SDL_SetRenderDrawColor(state.renderer, 20, 10, 30, 255);
		SDL_RenderClear(state.renderer);

		// REMEMBER: SDL executes in layers, so initialise it after clear to draw the background first before foreground material

		// Draw background images
		SDL_RenderTexture(state.renderer, res.texBg1, nullptr, nullptr);
		// Dividing scroll factors by half is the simpliest way
		drawParallaxBackground(state.renderer, res.texBg4, gs.player().velocity.x, gs.bg4Scroll, 0.075f, deltaTime);
		drawParallaxBackground(state.renderer, res.texBg3, gs.player().velocity.x, gs.bg3Scroll, 0.15f, deltaTime);
		drawParallaxBackground(state.renderer, res.texBg2, gs.player().velocity.x, gs.bg2Scroll, 0.3f, deltaTime);

		// Draw background tiles
		for (GameObject& obj : gs.backgroundTiles)
		{
			SDL_FRect dst{
				.x = obj.position.x - gs.mapViewport.x,
				.y = obj.position.y,
				.w = static_cast<float>(obj.texture->w),
				.h = static_cast<float>(obj.texture->h),
			};
			SDL_RenderTexture(state.renderer, obj.texture, nullptr, &dst);
		}

		// draw all objects
		for (auto& layer : gs.layers) {
			for (GameObject& obj : layer) {
				drawObject(state, gs, obj, TILE_SIZE, TILE_SIZE, deltaTime);
			}
		}

		// draw bullets
		for (GameObject& bullet : gs.bullets) {
			drawObject(state, gs, bullet, bullet.collider.w, bullet.collider.h, deltaTime);
		}

		// Draw foreground tiles
		for (GameObject& obj : gs.foregroundTiles)
		{
			SDL_FRect dst{
				.x = obj.position.x - gs.mapViewport.x,
				.y = obj.position.y,
				.w = static_cast<float>(obj.texture->w),
				.h = static_cast<float>(obj.texture->h),
			};
			SDL_RenderTexture(state.renderer, obj.texture, nullptr, &dst);
		}

		if (gs.debugMode) {
			// display debugo info
			SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
			// This is used to display states and debugs for visualisation, remember to put it in c_str format, {} indicate value to be placed
			SDL_RenderDebugText(state.renderer, 5, 5, std::format("S: {}, B: {}, G: {}",
				static_cast<int>(gs.player().data.player.state),
				gs.bullets.size(),
				gs.player().grounded).c_str());
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

	// Call VSync
	SDL_SetRenderVSync(state.renderer, 1);

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

void drawObject(const SDLState& state, GameState& gs, GameObject& obj, 
	float width, float height, float deltaTime) {
	
	float srcX = obj.currentAnimation != -1 ? obj.animations[obj.currentAnimation].currentFrame() * width : 0.0f;
	// Create a variable with SDL_FRect to specify part of the image to draw
	SDL_FRect src{
		.x = srcX,
		.y = 0,
		.w = width,
		.h = height,
	};

	SDL_FRect dst{
		// mapViewport is subtracted to allow for offset of camera over the map, without subtraction, our calculation of viewport has no effect
		// on the visuals
		.x = obj.position.x - gs.mapViewport.x,
		.y = obj.position.y,
		.w = width,
		.h = height,
	};

	SDL_FlipMode flipmode = obj.direction == -1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
	// Places the sprite and flips it depending on if the user is pressing the a or d key
	SDL_RenderTextureRotated(state.renderer, obj.texture, &src, &dst, 0, nullptr, flipmode);

	// This helps us visualise the hitboxes of all objects for collision debugging
	if (gs.debugMode) {
		SDL_FRect rectObj{
			.x = obj.position.x + obj.collider.x - gs.mapViewport.x,
			.y = obj.position.y + obj.collider.y,
			.w = obj.collider.w,
			.h = obj.collider.h,
		};
		SDL_SetRenderDrawColor(state.renderer, 255, 0, 0, 255);
		SDL_RenderFillRect(state.renderer, &rectObj);
	}
}

void update(const SDLState& state, GameState& gs, Resources& res, GameObject& obj, float deltaTime) {
	
	if (obj.dynamic) {
		// Gravity!
		obj.velocity += glm::vec2(0, 500) * deltaTime;
	}

	// Updates the player object
	if (obj.type == ObjectType::player) {
		float currentDirection = 0;
		if (state.keys[SDL_SCANCODE_A]) {
			currentDirection += -1;
		}
		if (state.keys[SDL_SCANCODE_D]) {
			currentDirection += 1;
		}
		if (currentDirection) {
			obj.direction = currentDirection;
		}

		// This is to make it easier to access that timer in the object rather than the whole of it
		Timer &weaponTimer = obj.data.player.weaponTimer;
		// Allows progression of time for rate of fire tracking
		weaponTimer.step(deltaTime);

		// Wrap it all in a lambda function for optimised compilation that lets the program determine if needed 
		const auto handleShooting = [&state, &gs, &res, &obj, &weaponTimer](SDL_Texture* tex, SDL_Texture *shootTex, int animIndex, int shootAnimIndex) {
			// Check if shoot key is pressed
			if (state.keys[SDL_SCANCODE_J]) {

				// set shooting tex/anim
				obj.texture = shootTex;
				obj.currentAnimation = shootAnimIndex;

				if (weaponTimer.isTimeout()) {
					weaponTimer.reset();
					// Spawn some bullets
					GameObject bullet;
					bullet.type = ObjectType::bullet;
					bullet.direction = gs.player().direction;
					bullet.texture = res.texBullet;
					bullet.currentAnimation = res.ANIM_BULLET_MOVING;
					bullet.collider = SDL_FRect{
						.x = 0,
						.y = 0,
						// We get both from height to simulate a square but also since width spans the entire animation
						.w = static_cast<float>(res.texBullet->h),
						.h = static_cast<float>(res.texBullet->h),
					};
					bullet.velocity = glm::vec2(
						obj.direction * (obj.velocity.x + 600.0f),
						0
					);
					bullet.animations = res.bulletAnims;

					// Adjust bullet start position
					const float left = 4;
					const float right = 24;
					const float t = (bullet.direction + 1) / 2.0f; // results in changing from -1 and 1 to 0 and 1
					// LERP or linear interpolation to find values between two known points
					// This case, it either puts x pos at left const or right const + 4 (signifying 4 pixels from right edge at TILE_SIZE 32)
					const float xOffset = left + right * t;

					bullet.position = glm::vec2(
						obj.position.x + xOffset,
						obj.position.y + TILE_SIZE / 2 + 1
					);
					gs.bullets.push_back(bullet);
				}
			}
			else {
				obj.texture = tex;
				obj.currentAnimation = animIndex;
			}
		};

		// Manages the different states of a player
		switch (obj.data.player.state) {
			case PlayerState::idle:
				if (currentDirection) {
					obj.data.player.state = PlayerState::running;
				}
				else {
					//deceleration
					if (obj.velocity.x) {
						const float factor = obj.velocity.x > 0 ? -1.5f : 1.5f;
						float amount = factor * obj.acceleration.x * deltaTime;
						if (std::abs(obj.velocity.x) < std::abs(amount)) {
							obj.velocity.x = 0;
						}
						else {
							obj.velocity.x += amount;
						}
					}
				}

				// Check for shooting
				handleShooting(res.texIdle, res.texShoot, res.ANIM_PLAYER_IDLE, res.ANIM_PLAYER_SHOOT);
				break;
			case PlayerState::running:
				// Changeing back to idle animation if no direction
				if (!currentDirection) {
					obj.data.player.state = PlayerState::idle;
				}

				// Moving in opposite direction of velocity to implement sliding
				if (obj.velocity.x * obj.direction < 0 && obj.grounded) {
					// Check if shooting and sliding
					handleShooting(res.texSlide, res.texSlideShoot, res.ANIM_PLAYER_SLIDE, res.ANIM_PLAYER_SLIDE_SHOOT);
				}
				else {
					// Check if shooting
					handleShooting(res.texRun, res.texRunShoot, res.ANIM_PLAYER_RUN, res.ANIM_PLAYER_SHOOT);
				}
				break;
			case PlayerState::jumping:
				// Check if shooting
				handleShooting(res.texRun, res.texRunShoot, res.ANIM_PLAYER_RUN, res.ANIM_PLAYER_SHOOT);
				break;
		}

		// Add acceleration into velocity
		obj.velocity += currentDirection * obj.acceleration * deltaTime;
		if (std::abs(obj.velocity.x) > obj.maxSpeedX) {
			obj.velocity.x = currentDirection * obj.maxSpeedX;
		}

	}
	// Add velocity to object position
	obj.position += obj.velocity * deltaTime;

	// Handle any collision detection
	bool foundGround = false;
	for (auto& layer : gs.layers) {
		for (GameObject& objB : layer) {
			if (&obj != &objB) {
				checkCollision(state, gs, res, obj, objB, deltaTime);

				// grounded sensor to check if object is on a ground by creating a small heigh rect just under the dynamic object
				SDL_FRect sensor{
					.x = obj.position.x + obj.collider.x,
					.y = obj.position.y + obj.collider.y + obj.collider.h,
					.w = obj.collider.w,
					.h = 1
				};
				SDL_FRect rectB{
					.x = objB.position.x + objB.collider.x,
					.y = objB.position.y + objB.collider.y,
					.w = objB.collider.w,
					.h = objB.collider.h,
				};
				if (SDL_HasRectIntersectionFloat(&sensor, &rectB)) {
					foundGround = true;
				}

			}
		}
	}
	if (obj.grounded != foundGround) {
		// Determines if state is grounded or airborne and adjust object as needed
		obj.grounded = foundGround;
		if (foundGround && obj.type == ObjectType::player) {
			obj.data.player.state = PlayerState::running;
		}
	}
}

void collisionResponse(const SDLState& state, GameState& gs, const Resources& res, const SDL_FRect& rectA,
	const SDL_FRect& rectB, const SDL_FRect& rectC, GameObject& a, GameObject& b, float deltaTime) {
	
	// object to be checked
	if (a.type == ObjectType::player) {
		// object that is colliding
		switch (b.type) {
			case ObjectType::level:
				if (rectC.w < rectC.h) {
					// horizontal collision
					// First check if any velocity is still present
					if (a.velocity.x > 0) {
						// Collision on the right of the player
						a.position.x -= rectC.w;
					}
					else if (a.velocity.x < 0) {
						// Collision on the left of player
						a.position.x += rectC.w;
					}
					// Stop from moving
					a.velocity.x = 0;
				}
				else {
					// vertical collision
					if (a.velocity.y > 0) {
						// Collision above the object
						a.position.y -= rectC.h;
					}
					else if (a.velocity.y < 0) {
						// Collision below the object
						a.position.y += rectC.h;
					}
					a.velocity.y = 0;
				}
				break;
		}
	}
}

void checkCollision(const SDLState& state, GameState& gs, const Resources& res, GameObject& a, GameObject& b, float deltaTime) {
	
	// A and B are objects in question, C is for the intersection rectangle
	// We add the collider as a way to represent the sprite within the TILE_SIZE to avoid misrepresentation of collision rects
	SDL_FRect rectA{
		.x = a.position.x + a.collider.x, 
		.y = a.position.y + a.collider.y,
		.w = a.collider.w,
		.h = a.collider.h,
	};
	SDL_FRect rectB{
		.x = b.position.x + b.collider.x,
		.y = b.position.y + b.collider.y,
		.w = b.collider.w,
		.h = b.collider.h,
	};
	SDL_FRect rectC{ 0 };

	// true if a SDL_FRect struct is returned
	if (SDL_GetRectIntersectionFloat(&rectA, &rectB, &rectC)) {
		collisionResponse(state, gs, res, rectA, rectB, rectC, a, b, deltaTime);
	}
}

void createTiles(const SDLState& state, GameState& gs, const Resources& res) {
	/*
		0 - Empty
		1 - Ground
		2 - Panel
		3 - Enemy
		4 - Player
		5 - Grass
		6 - Brick
	*/
	short map[MAP_ROWS][MAP_COLS] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 2, 0, 2, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	};

	short foreground[MAP_ROWS][MAP_COLS] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		5, 0, 0, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	};

	// Background can mainly be referred to as tiles that are surrounded by other tiles in the foreground/ map
	short background[MAP_ROWS][MAP_COLS] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		5, 0, 0, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	};

	// Lambda function to create each object quickly (notice that certain objects i.e. player have extra parameters to change)
	// [] represent capture clause which are local variables that lambda can access to modify, & operator indicate lambda to access existing variable instead of copying it
	const auto loadMap = [&state, &gs, &res](short layer[MAP_ROWS][MAP_COLS]) {
		const auto createObject = [&state](int r, int c, SDL_Texture* tex, ObjectType type) {
			GameObject o;
			o.type = type;
			o.position = glm::vec2(c * TILE_SIZE, state.logH - (MAP_ROWS - r) * TILE_SIZE);
			o.texture = tex;
			o.collider = {
				.x = 0,
				.y = 0,
				.w = TILE_SIZE,
				.h = TILE_SIZE,
			};
			return o;
			};

		for (int r = 0; r < MAP_ROWS; r++) {
			for (int c = 0; c < MAP_COLS; c++) {
				GameObject o;
				switch (layer[r][c]) {
					case 1:
						// Create ground object
						o = createObject(r, c, res.texGround, ObjectType::level);
						gs.layers[LAYER_IDX_LEVEL].push_back(o);
						break;
					case 2:
						// Create panel object
						o = createObject(r, c, res.texPanel, ObjectType::level);
						gs.layers[LAYER_IDX_LEVEL].push_back(o);
						break;
					case 4: {
						// Create player object
						GameObject player = createObject(r, c, res.texIdle, ObjectType::player);;
						// How we find the models current position
						player.data.player = PlayerData();
						player.animations = res.playerAnims;
						player.currentAnimation = res.ANIM_PLAYER_IDLE;
						player.acceleration = glm::vec2(300, 0);
						player.maxSpeedX = 100;
						player.dynamic = true;
						player.collider = {
							.x = 11,
							.y = 6,
							.w = 10,
							.h = 26
						};
						gs.layers[LAYER_IDX_CHARACTERS].push_back(player);
						gs.playerIndex = gs.layers[LAYER_IDX_CHARACTERS].size() - 1;
						break;
					}
					case 5:
						// Create grass object
						o = createObject(r, c, res.texGrass, ObjectType::level);
						gs.foregroundTiles.push_back(o);
						break;
					case 6:
						// Create brick object
						o = createObject(r, c, res.texBrick, ObjectType::level);
						gs.backgroundTiles.push_back(o);
						break;
				}
			}
		}
	};
	
	loadMap(map);
	loadMap(background);
	loadMap(foreground);
	// To ensure a player was initialized properly
	assert(gs.playerIndex != -1);
}

// Useful for handling one-case instances instead of continuous
void handleKeyInput(const SDLState& state, GameState& gs, GameObject& obj, SDL_Scancode key, bool keyDown) {
	
	const float JUMP_FORCE = -200.0f;
	
	// Check if player is jumping
	if (obj.type == ObjectType::player) {
		switch (obj.data.player.state) {
			case PlayerState::idle:
				if (key == SDL_SCANCODE_W && keyDown) {
					obj.data.player.state = PlayerState::jumping;
					obj.velocity.y += JUMP_FORCE;
				}
				break;
			case PlayerState::running:
				if (key == SDL_SCANCODE_W && keyDown) {
					obj.data.player.state = PlayerState::jumping;
					obj.velocity.y += JUMP_FORCE;
				}
				break;
		}
	}
}

// Move scroll position as inverse direction from player to imitate moving, so player going right means bkg going left
void drawParallaxBackground(SDL_Renderer* renderer, SDL_Texture* texture, float xVelocity, float& scrollPos, float scrollFactor, float deltaTime) {
	scrollPos -= xVelocity * scrollFactor * deltaTime;
	// If scrollPos is less than or equal to texture width, it snaps back to start of texture
	if (scrollPos <= -(texture->w)) {
		scrollPos = 0;
	}

	SDL_FRect dst{
		.x = scrollPos,
		.y = 35,
		// Making width double equivalent to rendering two textures side by side at once
		.w = texture->w * 2.0f,
		.h = static_cast<float>(texture->h),
	};

	SDL_RenderTextureTiled(renderer, texture, nullptr, 1, &dst);
}