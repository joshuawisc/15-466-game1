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

Load< TileAsset > player_tile(LoadTagDefault, [](){
	TileAsset const *ret =  new TileAsset(data_path("player.tile"));
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

PlayMode::PlayMode() {
	//TODO:
	// you *must* use an asset pipeline of some sort to generate tiles.
	// don't hardcode them like this!
	// or, at least, if you do hardcode them like this,
	//  make yourself a script that spits out the code that you paste in here
	//   and check that script into your repository.

	//Also, *don't* use these tiles in your game:

	// { //use tiles 0-16 as some weird dot pattern thing:
	// 	std::array< uint8_t, 8*8 > distance;
	// 	for (uint32_t y = 0; y < 8; ++y) {
	// 		for (uint32_t x = 0; x < 8; ++x) {
	// 			float d = glm::length(glm::vec2((x + 0.5f) - 4.0f, (y + 0.5f) - 4.0f));
	// 			d /= glm::length(glm::vec2(4.0f, 4.0f));
	// 			distance[x+8*y] = uint8_t(std::max(0,std::min(255,int32_t( 255.0f * d ))));
	// 		}
	// 	}
	// 	for (uint32_t index = 0; index < 16; ++index) {
	// 		PPU466::Tile tile;
	// 		uint8_t t = uint8_t((255 * index) / 16);
	// 		for (uint32_t y = 0; y < 8; ++y) {
	// 			uint8_t bit0 = 0;
	// 			uint8_t bit1 = 0;
	// 			for (uint32_t x = 0; x < 8; ++x) {
	// 				uint8_t d = distance[x+8*y];
	// 				if (d > t) {
	// 					bit0 |= (1 << x);
	// 				} else {
	// 					bit1 |= (1 << x);
	// 				}
	// 			}
	// 			tile.bit0[y] = bit0;
	// 			tile.bit1[y] = bit1;
	// 		}
	// 		ppu.tile_table[index] = tile;
	// 	}
	// }

	for (uint32_t x = 0; x < PPU466::BackgroundWidth; x++) {
		for (uint32_t y = 0; y < PPU466::BackgroundHeight; y++) {
			ppu.background[x+PPU466::BackgroundWidth*y] = 0 ;
		}
	}

	//use sprite 32 as a "player":
	// ppu.tile_table[32].bit0 = {
	// 	0b01111110,
	// 	0b11111111,
	// 	0b11111111,
	// 	0b11111111,
	// 	0b11111111,
	// 	0b11111111,
	// 	0b11111111,
	// 	0b01111110,
	// };
	// ppu.tile_table[32].bit1 = {
	// 	0b00000000,
	// 	0b00000000,
	// 	0b00011000,
	// 	0b00100100,
	// 	0b00000000,
	// 	0b00100100,
	// 	0b00000000,
	// 	0b00000000,
	// };
	ppu.tile_table[0] = back_tile->tile;
	ppu.tile_table[32] = player_tile->tile;
	ppu.tile_table[33] = ball_tile->tile;

	//makes the outside of tiles 0-16 solid:
	// ppu.palette_table[0] = {
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	// };

	//makes the center of tiles 0-16 solid:
	// ppu.palette_table[1] = {
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	// };

	// used for the background:
	ppu.palette_table[0] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0xff, 0xf1, 0xfa, 0xee),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	//used for the player:
	ppu.palette_table[7] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0xff, 0xff, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	//used for ball:
	ppu.palette_table[6] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x88, 0x88, 0xff, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	};

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

	player_at.x = PPU466::BackgroundWidth*2;
	player_at.y = PPU466::BackgroundHeight*2;

	ball_at.x = PPU466::BackgroundWidth/2;
	ball_at.y = PPU466::BackgroundWidth/10;

	return false;
}

void PlayMode::update(float elapsed) {

	//slowly rotates through [0,1):
	// (will be used to set background color)
	// background_fade += elapsed / 10.0f;
	// background_fade -= std::floor(background_fade);

	constexpr float PlayerSpeed = 60.0f;
	if (left.pressed) player_at.x -= PlayerSpeed * elapsed;
	if (right.pressed) player_at.x += PlayerSpeed * elapsed;
	if (down.pressed) player_at.y -= PlayerSpeed * elapsed;
	if (up.pressed) player_at.y += PlayerSpeed * elapsed;

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;

	constexpr float BallSpeed = 120.0f;

	static float BallSpeedX = 60.0f;
	static float BallSpeedY = 60.0f;

	ball_at.x += BallSpeedX * elapsed;
	ball_at.y += BallSpeedY * elapsed;
	if (ball_at.x >= PPU466::ScreenWidth-5)
		BallSpeedX *= -1;
	else if (ball_at.x <= 5)
		BallSpeedX *= -1;

	if (ball_at.y >= PPU466::ScreenHeight-5)
		BallSpeedY *= -1;
	else if (ball_at.y <= 5) {
		float x = player_at.x - ball_at.x;
		float y = player_at.y - ball_at.y;
		float root = sqrt(pow(x, 2) + pow(y, 2));
		BallSpeedX = (x/root) * BallSpeed;
		BallSpeedY = (y/root) * BallSpeed;
	}

}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---

	//background color will be some hsv-like fade:
	// ppu.background_color = glm::u8vec4(
	// 	std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 0.0f / 3.0f) ) ) ))),
	// 	std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 1.0f / 3.0f) ) ) ))),
	// 	std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 2.0f / 3.0f) ) ) ))),
	// 	0xff
	// );

	//tilemap gets recomputed every frame as some weird plasma thing:
	//NOTE: don't do this in your game! actually make a map or something :-)
	// for (uint32_t y = 0; y < PPU466::BackgroundHeight; ++y) {
	// 	for (uint32_t x = 0; x < PPU466::BackgroundWidth; ++x) {
	// 		//TODO: make weird plasma thing
	// 		ppu.background[x+PPU466::BackgroundWidth*y] = ((x+y)%16);
	// 	}
	// }

	//background scroll:
	// ppu.background_position.x = int32_t(-0.5f * player_at.x);
	// ppu.background_position.y = int32_t(-0.5f * player_at.y);

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

	//some other misc sprites:
	// for (uint32_t i = 1; i < 63; ++i) {
	// 	float amt = (i + 2.0f * background_fade) / 62.0f;
	// 	ppu.sprites[i].x = int8_t(0.5f * PPU466::ScreenWidth + std::cos( 2.0f * M_PI * amt * 5.0f + 0.01f * player_at.x) * 0.4f * PPU466::ScreenWidth);
	// 	ppu.sprites[i].y = int8_t(0.5f * PPU466::ScreenHeight + std::sin( 2.0f * M_PI * amt * 3.0f + 0.01f * player_at.y) * 0.4f * PPU466::ScreenWidth);
	// 	ppu.sprites[i].index = 32;
	// 	ppu.sprites[i].attributes = 6;
	// 	if (i % 2) ppu.sprites[i].attributes |= 0x80; //'behind' bit
	// }

	//--- actually draw ---
	ppu.draw(drawable_size);
}
