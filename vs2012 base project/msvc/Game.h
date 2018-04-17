#pragma once
#include <king/Engine.h>
#include <king/Updater.h>
#include "GameGem.h"
#include "Util.h"
#include <set>
#include "ChainGem.h"
#include <chrono>

using chrono_clock = std::chrono::steady_clock;

/************************************************************************/
/* Game class responsible for all the state machine of the game                                                                   
/************************************************************************/
class Game: public King::Updater
{
public:

	// Game status

	const enum Status { 
						IDLE,
						INITIAL,
						LOADING, 
						ON_MOUSE_BUTTON_DOWN,
						ON_MOUSE_BUTTON_UP,  
						SWAP,
						PROCESS_MATCH,
						END_PROCESSING,
						MATCH_AFTER_PROCESSING,
						REFILL_TOP,
						ANIMATE_FALLING,
						ANIMATE_NEW_GEMS, 
						END};
	Game();
	~Game();

	//Start the game
	void Start();

	//Update the game
	void Update();

	//Init all the necessary variables
	void Init();

	//Load windows
	void Load();

	// Render the windows
	void Render();

	// Swap the gems
	void Swap(short& fromRow, short& fromCol,short& toRow, short& toCol );

	// On release the click
	void OnMouseButtonUp(float& x, float& y);
	

	// Get the coordinates and row col from the point pressed
	void OnMouseButtonDown(float& x, float& y);

	
	// Generate a random gem that does not make a new chain
	King::Engine::Texture GetRandomGem(int row, int col);

	// Update row and coordinates to swap the gems
	void UpdateToSwap(short fromRow, short fromCol, short toRow, short toCol);

	//If the point touched is within the valid area of the board
	bool IsValidGem(float& x, float& y);

	// if the clicked was a valid one to the game
	bool IsValidCoordinates(const float& initial, const float& end, float& coord);

	// Transform coordinates on row col from the 'matrix' displayed
	void GetCoordinatesFromClick(const float coordX, const float coordY, short& row, short& col);

	// Get a new gem ( will obey to the no add new chain at that point )
	GameGem * GetNewGem(short row, short col);

	// If the swap produce a match 
	bool ItsAMatch(const short row, const short col);

	// Set the destination from the select and swapped gem
	void UpdateDestination(Position& from, Position& to);

	//Locate all the matches on the board
	void FindMatches();

private:

	// If the swapped gem has a match horizontally
	bool isHorizontalMatch(King::Engine::Texture texture, const short row, const short col);

	// If the swapped gems has a match vertically
	bool isVerticalMatch(King::Engine::Texture texture, const short row, const short col);

	//get the horizontal chain
	void HorizontalMatches();

	void HorizontalMatches(GameGem * const gems[Util::ROWS][Util::COLS]);

	//get the vertical chain
	void VerticalMatches();

	//get the vertical chain
	void VerticalMatches(GameGem * const gems[Util::COLS][Util::ROWS]);

	// remove the matches from the board
	void RemoveMatches();

	//Fill the void left fot the matches
	void FillHoles();

	// Animate the gems falling 
	void AnimateGems(std::vector<GameGem*>& animate);
	
	//Create new gems to the top
	void RefillTop();

	// Updathe the game score
	void UpdateScore(short chainSize);

	// Shows the gem falling from the top ( new gems )
	void AnimateGemsTop();
	
	// Game status machine
	Game::Status mStatus;
	// King engine
	King::Engine mEngine;
	
	//matrix that contains the gems 
	GameGem * mGems[Util::ROWS][Util::COLS];

	// chains to be deleted from the display
	std::vector<ChainGem*> mChains;

	// vector to animations
	std::vector<GameGem*> mFallingAnimation;
	std::vector<GameGem*> mRefillingAnimation;
	std::vector<GameGem*> mDoneAnimation;

	// Points clicked and correspondent row and col from the gems that are going to try to swapped
	float mClickCoordX, mClickCoordY, mReleaseCoordX,mReleaseCoordY;
	short mFromRow, mFromCol, mToRow, mToCol;

	// Its not a valid swap, then swap back to originals 
	bool mSwapBack;
	// Waiitng for the button to be released
	bool mWaitingMouseUp;
	// fill the holes on the board with gems, doing it after the match process
	bool mFillHoles;

	// timer the game started
	chrono_clock::time_point mStart;
	// Game score
	short mScore;

	// Game is going to end.
	bool mIsGoingToEnd;

};

