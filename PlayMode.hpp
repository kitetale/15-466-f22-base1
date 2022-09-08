#include "PPU466.hpp"
#include "Mode.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up, space;

	//some weird background animation:
	float background_fade = 0.0f;

	//player speed 
	float PlayerSpeed = 30.0f;

	//player position:
	glm::vec2 player_at = glm::vec2(30.0f,10.0f);

	//leaf position:
	glm::vec2 leaf = glm::vec2(100.0f,100.0f);

	//ate leaf:
	bool scored = false;

	//keeping track of score:
	uint16_t score = 0;
	uint16_t score1s = 45; //tile index for 0
	uint16_t score10s = 45; //tile index for 0

	//each tile location
	// starting with front leg location (head above front leg)
	std::deque < glm::vec2 > positions {glm::vec2(27.0f, 3.0f),glm::vec2(19.0f, 3.0f),glm::vec2(11.0f, 3.0f)};


	//----- drawing handled by PPU466 -----

	PPU466 ppu;
};
