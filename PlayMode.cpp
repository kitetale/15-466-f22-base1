#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

//for load_png :
#include "load_save_png.hpp"

#include <random>

PlayMode::PlayMode() {
	//TODO:
	// you *must* use an asset pipeline of some sort to generate tiles.
	// don't hardcode them like this!
	// or, at least, if you do hardcode them like this,
	//  make yourself a script that spits out the code that you paste in here
	//   and check that script into your repository.

	//Also, *don't* use these tiles in your game:

	{ //use tiles 0-16 as some weird dot pattern thing:
		for (uint32_t index = 0; index < 16; ++index) {
			PPU466::Tile tile;

			for (uint32_t y = 0; y < 8; ++y) {
				uint8_t bit0 = 0;
				uint8_t bit1 = 0;
				for (uint32_t x = 0; x < 8; ++x) {
					
				}
				tile.bit0[y] = bit0;
				tile.bit1[y] = bit1;
			}
			ppu.tile_table[index] = tile;
		}
	}

	//use sprite 32 as a "cat head":
	
	ppu.tile_table[32].bit0 = {
		0,0,0,0,0,0,0,0
	};
	ppu.tile_table[32].bit1 = {
		0,0,0,0,0,0,0,0
	};

	//used for the cat face (player):
	ppu.palette_table[7] = {
		glm::u8vec4(0xff, 0xab, 0x3a, 0xff), //orange
		glm::u8vec4(0xff, 0xde, 0xb2, 0xff), //light orange
		glm::u8vec4(0x20, 0x19, 0x19, 0xff), //black
		glm::u8vec4(0x00, 0x00, 0x00, 0x00), //clear
	};
	// cat body parts
	ppu.palette_table[6] = {
		glm::u8vec4(0xff, 0xab, 0x3a, 0xff), //orange
		glm::u8vec4(0xff, 0xde, 0xb2, 0xff), //light orange
		glm::u8vec4(0x81, 0x4a, 0x00, 0xff), //brown
		glm::u8vec4(0x00, 0x00, 0x00, 0x00), //clear
	};

	glm::uvec2 size (16,16);
	std::vector< glm::u8vec4 > data;
	load_png("catto.png", &size, &data, LowerLeftOrigin); 
	
	// data contains pixel color info (all 4 tiles)
	// 1. loop through and compare to find index of color
	// sprite 32 is cat head 
	// sprite 0, 1, 2 are cat body, leg, tail

	for (uint32_t index_i = 0; index_i < 8; ++index_i) { // col
		for (uint32_t index_j = 0; index_j < 8; ++index_j) { //row
			glm::u8vec4 pixel = data[index_j + index_i * 16];
			for (uint16_t index_k = 0; index_k < 4; ++index_k) { // index in palette
				if (ppu.palette_table[7][index_k] == pixel) {
					// found the index on palette
					// bit0 = index_k & 0x1
					// bit1 = (index_k >> 1) & 0x1
					ppu.tile_table[32].bit0[index_i] = (ppu.tile_table[32].bit0[index_i]<<1) | (index_k & 0x1);
					ppu.tile_table[32].bit1[index_i] = (ppu.tile_table[32].bit1[index_i]<<1) | ((index_k >> 1) & 0x1);
				}
			}
		}
	}

	

	//makes the outside of tiles 0-16 solid:
	ppu.palette_table[0] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	//makes the center of tiles 0-16 solid:
	ppu.palette_table[1] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.downs += 1;
			left.pressed = true;
			// set other keys to false
			right.pressed = false;
			up.pressed = false;
			down.pressed = false;
			space.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.downs += 1;
			right.pressed = true;
			// set other keys to false
			left.pressed = false;
			up.pressed = false;
			down.pressed = false;
			space.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.downs += 1;
			up.pressed = true;
			// set other keys to false
			left.pressed = false;
			right.pressed = false;
			down.pressed = false;
			space.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.downs += 1;
			down.pressed = true;
			// set other keys to false
			left.pressed = false;
			right.pressed = false;
			up.pressed = false;
			space.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_SPACE) {
			space.downs += 1;
			space.pressed = true;
			// set other keys to false
			left.pressed = false;
			right.pressed = false;
			up.pressed = false;
			down.pressed = false;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	constexpr float PlayerSpeed = 30.0f;
	if (left.pressed) player_at.x -= PlayerSpeed * elapsed;
	if (right.pressed) player_at.x += PlayerSpeed * elapsed;
	if (down.pressed) player_at.y -= PlayerSpeed * elapsed;
	if (up.pressed) player_at.y += PlayerSpeed * elapsed;

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---

	//background color will be some hsv-like fade:
	ppu.background_color = glm::u8vec4(
		0xFFB98F, //orange pastel
		0xFF9B86, //redish pastel
		0xD8D6D6, //light gray
		0xff // white
	);

	for (uint32_t y = 0; y < PPU466::BackgroundHeight; ++y) {
		for (uint32_t x = 0; x < PPU466::BackgroundWidth; ++x) {
			if (x%4==0) {
				ppu.background[x+PPU466::BackgroundWidth*y] = 0;
			} else if (y%4==0) {
				ppu.background[x+PPU466::BackgroundWidth*y] = 1;
			}else ppu.background[x+PPU466::BackgroundWidth*y] = 15;
		}
	}

	//background scroll:
	ppu.background_position.x = int32_t(-0.5f * player_at.x);
	ppu.background_position.y = int32_t(-0.5f * player_at.y);

	//player sprite:
	ppu.sprites[0].x = int8_t(player_at.x);
	ppu.sprites[0].y = int8_t(player_at.y);
	ppu.sprites[0].index = 32;
	ppu.sprites[0].attributes = 7;

	//--- actually draw ---
	ppu.draw(drawable_size);
}
