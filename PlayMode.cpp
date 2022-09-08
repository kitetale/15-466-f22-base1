#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

//for load_png :
#include "load_save_png.hpp"

#include <random>

PlayMode::PlayMode() {
	space.pressed = 1;

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

	//use sprite 32-47
	//reset table for these
	for (uint16_t index = 32; index < 48; ++index){
		ppu.tile_table[index].bit0 = {
			0,0,0,0,0,0,0,0
		};
		ppu.tile_table[index].bit1 = {
			0,0,0,0,0,0,0,0
		};
	}
	// PALETTE TABLE INITS

	// background
	ppu.palette_table[0] = {
		glm::u8vec4(0x9a, 0xc3, 0xff, 0xff), //blue
		glm::u8vec4(0xe4, 0xb7, 0xb7, 0xff), //pink
		glm::u8vec4(0xc4, 0x9a, 0xff, 0xff), //purple
		glm::u8vec4(0x00, 0x00, 0x00, 0x00), //clear
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
	// leaf
	ppu.palette_table[5] = {
		glm::u8vec4(0x54, 0x88, 0x33, 0xff), //green
		glm::u8vec4(0x35, 0x53, 0x22, 0xff), //dark green
		glm::u8vec4(0x81, 0x4a, 0x00, 0xff), //brown
		glm::u8vec4(0x00, 0x00, 0x00, 0x00), //clear
	};

	glm::uvec2 size (32,32);
	std::vector< glm::u8vec4 > data;
	load_png("catto_num_leaf.png", &size, &data, LowerLeftOrigin); 
	
	// data contains pixel color info (all 4 tiles) from catto.png
	// 1. loop through and compare to find index of color
	// sprite 32 is cat head 
	// sprite 33, 34, 35 are cat body, leg, tail

	// cat 
	// sprite location on catto_num_leaf.png : 
	// 44   45   46c  47
	// 40   41   42   43
	// 36c  37c  38   39
	// 32c  33c  34   35

	// note : realized I flipped row & col variable name compared to convention -- but logic still holds!
	// r: 1 2 3 4
	// c: 4
	//    3
	//    2
	//    1
	uint16_t table_index = 7;
	for (uint16_t col = 0; col < 4; ++col) { // col tile
		for (uint16_t row = 0; row < 4; ++row) { // row tile
			if (col==3 && row==2) table_index = 6; //change palette to get brown
			if (col==3 && row==3) table_index = 5; //change palette to get leaf
			for (uint16_t index_i = 0; index_i < 8; ++index_i){ // col
				for (uint16_t index_j = 0; index_j < 8; ++index_j){ // row
					glm::u8vec4 pixel = data[(index_j + row*8) + (index_i + col*8) *32];
					for (uint16_t index_k = 0; index_k < 4; ++index_k) { //index in palette
						if (ppu.palette_table[table_index][index_k] == pixel) {
							// found the index on palette
							// bit0 = index_k & 0x1
							// bit1 = (index_k >> 1) & 0x1

							// left-right inverted:
							// ppu.tile_table[32].bit0[index_i] = (ppu.tile_table[32].bit0[index_i]<<1) | (index_k & 0x1);
							// ppu.tile_table[32].bit1[index_i] = (ppu.tile_table[32].bit1[index_i]<<1) | ((index_k >> 1) & 0x1);

							// correct orientation
							ppu.tile_table[32+row+col*4].bit0[index_i] = ((index_k & 0x1) << index_j) | ppu.tile_table[32+row+col*4].bit0[index_i];
							ppu.tile_table[32+row+col*4].bit1[index_i] = (((index_k >> 1) & 0x1) << index_j) | ppu.tile_table[32+row+col*4].bit1[index_i];
						}
					}
				}
			}
		}
	}

	//background tiles (3 colors of tiles)
	for (uint16_t index = 0; index < 8; ++index) {
		ppu.tile_table[0].bit0[index] = 0b00000000;
		ppu.tile_table[1].bit0[index] = 0b11111111;
		ppu.tile_table[2].bit0[index] = 0b00000000;

		ppu.tile_table[0].bit1[index] = 0b00000000;
		ppu.tile_table[1].bit1[index] = 0b00000000;
		ppu.tile_table[2].bit1[index] = 0b11111111;
	}

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
	if (left.pressed) player_at.x -= PlayerSpeed * elapsed;
	if (right.pressed) player_at.x += PlayerSpeed * elapsed;
	if (down.pressed) player_at.y -= PlayerSpeed * elapsed;
	if (up.pressed) player_at.y += PlayerSpeed * elapsed;

	if (player_at.x <= 16+threshold*8 || player_at.x >= PPU466::ScreenWidth
		|| player_at.y <= 0 || player_at.y >= PPU466::ScreenHeight) {
			player_at = glm::vec2(PPU466::ScreenWidth/2, PPU466::ScreenHeight/2);
			space.pressed = true;
			left.pressed = false;
			right.pressed = false;
			up.pressed = false;
			down.pressed = false;
			if (score > 0) --score;
			if (threshold > 0) --threshold;
			else {
				score = 0;
				threshold = 0;
			}
			std::cout << "No going out of window!" << std::endl;
		}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;

	// update score if ate leaf
	if (player_at.x > leaf.x-4 &&
		player_at.x < leaf.x+4 &&
		player_at.y > leaf.y-4 &&
		player_at.y < leaf.y+4 &&
		score < 100) {
			++score;
			++threshold;
			PlayerSpeed += 5.0f;
			scored = true;

			// update score sprite tile index
			switch (score % 10) {
				case 1: score1s = 34; break;
				case 2: score1s = 35; break;
				case 3: score1s = 39; break;
				case 4: score1s = 38; break;
				case 5: score1s = 40; break;
				case 6: score1s = 41; break;
				case 7: score1s = 42; break;
				case 8: score1s = 43; break;
				case 9: score1s = 44; break;
				default: score1s = 45; break;
			}
			switch (score / 10) {
				case 1: score10s = 34; break;
				case 2: score10s = 35; break;
				case 3: score10s = 39; break;
				case 4: score10s = 38; break;
				case 5: score10s = 40; break;
				case 6: score10s = 41; break;
				case 7: score10s = 42; break;
				case 8: score10s = 43; break;
				case 9: score10s = 44; break;
				default: score10s = 45; break;
			}
		}
		// decrement score if ate rotten
		if (player_at.x > rotten.x-4 &&
		player_at.x < rotten.x+4 &&
		player_at.y > rotten.y-4 &&
		player_at.y < rotten.y+4 &&
		score < 100) {
			if (score <= 0) {
				player_at = glm::vec2(30.0f,10.0f);
				space.pressed = true;
				left.pressed = false;
				right.pressed = false;
				up.pressed = false;
				down.pressed = false;
				std::cout << "GAME OVER! Starting new game." << std::endl;
			} else {
				if (threshold < 16) --score; 
				--threshold;
				PlayerSpeed -= 5.0f;
				dec_score = true;

				// update score sprite tile index
				switch (score % 10) {
					case 1: score1s = 34; break;
					case 2: score1s = 35; break;
					case 3: score1s = 39; break;
					case 4: score1s = 38; break;
					case 5: score1s = 40; break;
					case 6: score1s = 41; break;
					case 7: score1s = 42; break;
					case 8: score1s = 43; break;
					case 9: score1s = 44; break;
					default: score1s = 45; break;
				}
				switch (score / 10) {
					case 1: score10s = 34; break;
					case 2: score10s = 35; break;
					case 3: score10s = 39; break;
					case 4: score10s = 38; break;
					case 5: score10s = 40; break;
					case 6: score10s = 41; break;
					case 7: score10s = 42; break;
					case 8: score10s = 43; break;
					case 9: score10s = 44; break;
					default: score10s = 45; break;
				}
			}
		}

	// leaf loc
	if (scored) {
		//randomize next leaf location
		leaf.x = rand() % 100 + 50;
		leaf.y = rand() % 200 + 10;
	
		scored = false;
	}
	// rotten loc
	if (dec_score) {
		//randomize next rotten location
		rotten.x = rand() % 100 + 50;
		rotten.y = rand() % 170 + 10;

		dec_score = false;
	}

}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---

	for (uint32_t y = 0; y < PPU466::BackgroundHeight; ++y) {
		for (uint32_t x = 0; x < PPU466::BackgroundWidth; ++x) {
			// plaid pattern #
			// if (x%4==2) {
			// 	ppu.background[x+PPU466::BackgroundWidth*y] = 1;
			// } else if (y%4==1) {
			// 	ppu.background[x+PPU466::BackgroundWidth*y] = 2;
			// }else ppu.background[x+PPU466::BackgroundWidth*y] = 0;
			ppu.background[x+PPU466::BackgroundWidth*y] = score10s % 3;
		}
	}

	// if want background scroll:
	// ppu.background_position.x = int32_t(-0.5f * player_at.x);
	// ppu.background_position.y = int32_t(-0.5f * player_at.y);

	//player sprite:
	// 0: head 
	// 1: front leg
	// 2: body
	// 59: back leg
	// 60: tail
	// 61: score digit 1
	// 62: score digit 10
	// 63: leaf
	// cat head
	ppu.sprites[0].x = int8_t(player_at.x);
	ppu.sprites[0].y = int8_t(player_at.y);
	ppu.sprites[0].index = 32;
	ppu.sprites[0].attributes = 7;
	// cat front leg
	ppu.sprites[1].x = int8_t(player_at.x-3);
	ppu.sprites[1].y = int8_t(player_at.y-7);
	ppu.sprites[1].index = 33;
	ppu.sprites[1].attributes = 7;
	// cat body
	ppu.sprites[2].x = int8_t(player_at.x-11);
	ppu.sprites[2].y = int8_t(player_at.y-7);
	ppu.sprites[2].index = 46;
	ppu.sprites[2].attributes = 6;

	//cat body based on score
	for (uint16_t body = 0; body < threshold; ++body) {
		ppu.sprites[body+3].x = int8_t(player_at.x-19-(body*8)); //+3 to offset head/front leg sprite
		ppu.sprites[body+3].y = int8_t(player_at.y-7);
		ppu.sprites[body+2].index = 46;
		ppu.sprites[body+2].attributes = 6;
	}

	// cat back leg
	ppu.sprites[59].x = int8_t(player_at.x-11-(score*8));
	ppu.sprites[59].y = int8_t(player_at.y-7);
	ppu.sprites[59].index = 36;
	ppu.sprites[59].attributes = 7;
	// cat tail
	ppu.sprites[60].x = int8_t(player_at.x-19-(score*8));
	ppu.sprites[60].y = int8_t(player_at.y-7);
	ppu.sprites[60].index = 37;
	ppu.sprites[60].attributes = 7;

	// leaf sprite at random location:
	ppu.sprites[63].x = int8_t(leaf.x);
	ppu.sprites[63].y = int8_t(leaf.y);
	ppu.sprites[63].index = 47;
	ppu.sprites[63].attributes = 5;

	// rotten leaf sprite at random location:
	ppu.sprites[58].x = int8_t(rotten.x);
	ppu.sprites[58].y = int8_t(rotten.y);
	ppu.sprites[58].index = 47;
	ppu.sprites[58].attributes = 6;

	// score:
	ppu.sprites[61].x = (PPU466::ScreenWidth-20);
	ppu.sprites[61].y = (PPU466::ScreenHeight-20);
	ppu.sprites[61].index = score1s;
	ppu.sprites[61].attributes = 7;

	ppu.sprites[62].x = (PPU466::ScreenWidth-28);
	ppu.sprites[62].y = (PPU466::ScreenHeight-20);
	ppu.sprites[62].index = score10s;
	ppu.sprites[62].attributes = 7;

	//--- actually draw ---
	ppu.draw(drawable_size);
}

// Questions:
// - Palette table index not switching palette.. //change .attributes palette index
// - How does background works?