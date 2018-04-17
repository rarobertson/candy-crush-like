#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string>


class Util
{
public:

	static constexpr auto ROWS=8;
	static constexpr auto COLS=8;

	static constexpr auto GEM_WIDTH = 35.0f;
	static constexpr auto GEM_HEIGHT = 35.0f;
	
	static constexpr auto GEM_OFFSET_X = 5.0f;
	static constexpr auto GEM_OFFSET_Y = 5.0f;
	
	static constexpr auto POSX_BOARD_INITIAL = 340.0f;
	static constexpr auto POSX_BOARD_END = POSX_BOARD_INITIAL+ROWS*(GEM_WIDTH+GEM_OFFSET_X);
	static constexpr auto POSY_BOARD_INITIAL = 110.0f;
	static constexpr auto POSY_BOARD_END = POSY_BOARD_INITIAL + ROWS*(GEM_HEIGHT + GEM_OFFSET_Y);

	static constexpr auto CHAIN_SIZE = 3;

	static constexpr auto ASSETS = "./assets";
	static constexpr auto TIMER = "TIME LEFT: ";
	static constexpr auto SCORE = "SCORE: ";


	static constexpr auto POSX_TIMER = 55.0f;
	static constexpr auto POSY_TIMER = 140.0f;

	static constexpr auto POSX_SCORE = 55.0f;
	static constexpr auto POSY_SCORE = 180.0f;

	static constexpr auto INITIAL_TIME = 60;

	static constexpr auto SMALL_CHAIN = 3;
	static constexpr auto MEDIUM_CHAIN = 4;
	static constexpr auto LARGE_CHAIN = 5;
	static constexpr auto SCORE_SMALL_CHAIN = 50;
	static constexpr auto SCORE_MEDIUM_CHAIN = 100;
	static constexpr auto SCORE_LARGE_CHAIN = 200;


};

