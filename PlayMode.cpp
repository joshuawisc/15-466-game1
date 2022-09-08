#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

//For asset loading:
#include "Load.hpp"

#include "TileAsset.cpp"

#include <cmath>
#include <random>
#include <algorithm>

// From discord comment: https://discord.com/channels/1013997989217435679/1013998948232470610/1015992136426070086
Load< TileAsset > player_tile(LoadTagDefault, [](){
	TileAsset const *ret =  new TileAsset(data_path("player2.tile"));
	return ret;
});

Load< TileAsset > ball_tile(LoadTagDefault, [](){
	TileAsset const *ret =  new TileAsset(data_path("ball.tile"));
	return ret;
});

Load< TileAsset > back_tile(LoadTagDefault, [](){
	TileAsset const *ret =  new TileAsset(data_path("back.tile"));
	return ret;
});

Load< TileAsset > death_tile(LoadTagDefault, [](){
	TileAsset const *ret =  new TileAsset(data_path("death.tile"));
	return ret;
});

Load< TileAsset > brick_tile(LoadTagDefault, [](){
	TileAsset const *ret =  new TileAsset(data_path("brick.tile"));
	return ret;
});

Load< TileAsset > l_tile(LoadTagDefault, [](){
	TileAsset const *ret =  new TileAsset(data_path("l.tile"));
	return ret;
});

Load< TileAsset > o_tile(LoadTagDefault, [](){
	TileAsset const *ret =  new TileAsset(data_path("o.tile"));
	return ret;
});

Load< TileAsset > s_tile(LoadTagDefault, [](){
	TileAsset const *ret =  new TileAsset(data_path("s.tile"));
	return ret;
});

Load< TileAsset > e_tile(LoadTagDefault, [](){
	TileAsset const *ret =  new TileAsset(data_path("e.tile"));
	return ret;
});

Load< TileAsset > r_tile(LoadTagDefault, [](){
	TileAsset const *ret =  new TileAsset(data_path("r.tile"));
	return ret;
});

PlayMode::PlayMode() {
	//TODO:
	// you *must* use an asset pipeline of some sort to generate tiles.
	// don't hardcode them like this!
	// or, at least, if you do hardcode them like this,
	//  make yourself a script that spits out the code that you paste in here
	//   and check that script into your repository.


	for (uint32_t x = 0; x < PPU466::BackgroundWidth; x++) {
		for (uint32_t y = 0; y < PPU466::BackgroundHeight; y++) {
			if (y >= 17 && y <= 19) {
				ppu.background[x+PPU466::BackgroundWidth*y] = 0 | 256 ;
			} else {
				ppu.background[x+PPU466::BackgroundWidth*y] = 1 ;
			}
		}

	}

	ppu.tile_table[0] = brick_tile->tile;
	ppu.tile_table[1] = back_tile->tile;
	ppu.tile_table[2] = l_tile->tile;
	ppu.tile_table[3] = o_tile->tile;
	ppu.tile_table[4] = s_tile->tile;
	ppu.tile_table[5] = e_tile->tile;
	ppu.tile_table[6] = r_tile->tile;
	ppu.tile_table[32] = player_tile->tile;
	ppu.tile_table[33] = ball_tile->tile;


	// used for the background:
	ppu.palette_table[0] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0xf1, 0xfa, 0xee, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	//used for bricks / letter:
	ppu.palette_table[1] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x48, 0x95, 0xef, 0xff),
		glm::u8vec4(0x4c, 0xc9, 0xf0, 0xff),
		glm::u8vec4(0xff, 0x00, 0x00, 0xff),
	};

	//used for spaces:
	ppu.palette_table[2] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x48, 0x95, 0xef, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	//used for the player:
	ppu.palette_table[7] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x1d, 0x35, 0x57, 0xff),
		glm::u8vec4(0xa8, 0xda, 0xdc, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	//used for ball:
	ppu.palette_table[6] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0xe6, 0x39, 0x46, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	};

	player_at.x = PPU466::ScreenWidth/2;
	player_at.y = PPU466::ScreenHeight/2;

	ball_at.x = PPU466::ScreenWidth/2;
	ball_at.y = PPU466::ScreenHeight/10;

}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.downs += 1;
			down.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.pressed = false;
			return true;
		}
	}


	return false;
}

void PlayMode::update(float elapsed) {

	static float PlayerSpeed = 60.0f;

	static float BallSpeed = 200.0f;

	static float BallSpeedX = 60.0f;
	static float BallSpeedY = 60.0f;


	auto dist = [=](glm::vec2 a, glm::vec2 b) {
		float x = b.x - a.x;
		float y = b.y - a.y;
		return sqrt(pow(x, 2) + pow(y, 2));
	};

	if (dist(player_at, ball_at) < 10) {
		lost = true;
		ppu.tile_table[32] = death_tile->tile;
		BallSpeed = 60.0f;
		int index = 0;

		// Lose state background
		for (uint32_t y = 0; y < PPU466::BackgroundHeight; y++) {
			index = (y%2)*2;
			for (uint32_t x = 0; x < PPU466::BackgroundWidth; x++) {
			 	uint16_t new_val = (((index++)%6)+1) | 256;
				if (new_val == 257)
					new_val = 1 | 512;
				ppu.background[x+PPU466::BackgroundWidth*y] = new_val;
			}
		}
	}

	if (lost) {
		ppu.background_position.x += 1;
	}

	if (left.pressed) player_at.x -= PlayerSpeed * elapsed;
	if (right.pressed) player_at.x += PlayerSpeed * elapsed;
	if (down.pressed) player_at.y -= PlayerSpeed * elapsed;
	if (up.pressed) player_at.y += PlayerSpeed * elapsed;

	// Don't allow wrapping
	player_at.x = std::max(player_at.x, 0.0f);
	player_at.x = std::min(player_at.x, float(PPU466::ScreenWidth)-5);
	player_at.y = std::max(player_at.y, 0.0f);
	player_at.y = std::min(player_at.y, float(PPU466::ScreenHeight)-5);

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;


	ball_at.x += BallSpeedX * elapsed;
	ball_at.y += BallSpeedY * elapsed;
	if (ball_at.x >= PPU466::ScreenWidth-5)
		BallSpeedX *= -1;
	else if (ball_at.x <= 2)
		BallSpeedX *= -1;

	if (ball_at.y >= PPU466::ScreenHeight-5 || ball_at.y <= 2) {
		float x = player_at.x - ball_at.x;
		float y = player_at.y - ball_at.y;
		float root = float(sqrt(pow(x, 2) + pow(y, 2)));
		BallSpeedX = (x/root) * BallSpeed;
		BallSpeedY = (y/root) * BallSpeed;
	}

	// Check if brick hit
	int brick_x = int(ball_at.x/8);
	int brick_y = int(ball_at.y/8);
	if (ppu.background[brick_x+PPU466::BackgroundWidth*brick_y] ==  (0 | 256 )) {
		ppu.background[brick_x+PPU466::BackgroundWidth*brick_y] = 1;
		BallSpeedY *= -1;
	}

}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---


	//player sprite:
	ppu.sprites[0].x = int8_t(player_at.x);
	ppu.sprites[0].y = int8_t(player_at.y);
	ppu.sprites[0].index = 32;
	ppu.sprites[0].attributes = 7;

	//ball sprite:
	ppu.sprites[1].x = int8_t(ball_at.x);
	ppu.sprites[1].y = int8_t(ball_at.y);
	ppu.sprites[1].index = 33;
	ppu.sprites[1].attributes = 6;


	//--- actually draw ---
	ppu.draw(drawable_size);
}
