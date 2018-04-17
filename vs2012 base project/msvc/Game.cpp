#include "Game.h"
#include <time.h>
#include <iostream>
#include "SwapGem.h"
#include "ChainGem.h"
#include <string>
#include <thread>

float a = 0;
float b = 0;
int cont2 = 0;

Game::Game():
	mStatus(Status::INITIAL),
	mEngine(Util::ASSETS),
	mFromCol(0),
	mToCol(0),
	mFromRow(0),
	mToRow(0),
	mSwapBack(false),
	mWaitingMouseUp(false),
	mClickCoordX(0.0f),
	mClickCoordY(0.0f),
	mReleaseCoordX(0.0f),
	mReleaseCoordY(0.0f),
	mScore(0)
{
}

Game::~Game()
{
	for (auto row = 0; row < Util::ROWS; row++) {
		for (auto col = 0; col < Util::COLS; col++) {
			delete mGems[row][col];
		}
	}
}

void Game::Start()
{

	mStatus = Game::Status::LOADING;
	Init();
	mEngine.Start(*this);
}

void Game::Update()
{

	if (mEngine.GetMouseButtonDown() && !mWaitingMouseUp && mRefillingAnimation.empty() && mFallingAnimation.empty()) {
		// If is mouse down and its not waiting mouse up
		mStatus = Game::ON_MOUSE_BUTTON_DOWN;
	}
	
	if (mEngine.GetMouseButtonUp()) {
		if (mStatus == ON_MOUSE_BUTTON_DOWN && SwapGem::GetInstance().SwapStatus() == SwapGem::Swapping::STARTED) {
			//If mouse up ( do the swap ) and its valid 
			mStatus = Game::ON_MOUSE_BUTTON_UP;
			mWaitingMouseUp = false;
		}

	}
	


	switch (mStatus)
	{

		case Game::INITIAL:
			break;

		case Game::LOADING:
			Load();
			mStatus = Game::IDLE;
			break;
		case Game::IDLE:
			mFromRow = 0;
			mToRow = 0;
			mFromCol = 0;
			mToCol = 0;
			mClickCoordX = 0.0f;
			mClickCoordY = 0.0f;
			mReleaseCoordX = 0.0f;
			mReleaseCoordY = 0.0f;

			if (mIsGoingToEnd) {
				/*std::cout << "ENDING..." << std::endl;
				mEngine.Quit()
				//mStatus = Game::END;
				//mStatus = Game::END;*/
			}
			break;

		case Game::ON_MOUSE_BUTTON_UP:
			
			// Call the mouse down and get the valid coordinates to swap
			OnMouseButtonUp(mReleaseCoordX, mReleaseCoordY);
			break;

		case Game::ON_MOUSE_BUTTON_DOWN:
			
			// Call the mouse down and get the valid coordinates from swap
			OnMouseButtonDown(mClickCoordX, mClickCoordY);
			break;
		
		case Game::SWAP:

			// Swap the gems 
			if (SwapGem::GetInstance().SwapStatus() == SwapGem::END){
				if (mSwapBack) {
					Swap(mFromRow, mFromCol, mToRow, mToCol);					
				}
				mStatus = mSwapBack == false ? Game::PROCESS_MATCH : Game::IDLE;
				mSwapBack = false;
			}
			else if (SwapGem::GetInstance().SwapStatus() != SwapGem::END) {
				
				// Update the position the position and do the swap animation
				Position from = mGems[mFromRow][mFromCol]->GetPosition();
				Position to = mGems[mToRow][mToCol]->GetPosition();
				
				SwapGem::GetInstance().UpdateSwap(from, to);
				
				mGems[mFromRow][mFromCol]->SetPosition(from);
				mGems[mToRow][mToCol]->SetPosition(to);

			}
			break;

		case Game::PROCESS_MATCH:
			//std::cout << "ITS A ME A MATCHRIO!!!" << std::endl;
			
			// At the end of swap animation check if its a valid swap 
			// and switch the "logical" gem as well if not swap back

			Swap(mFromRow, mFromCol, mToRow, mToCol);

			if ( ItsAMatch(mFromRow, mFromCol) || ItsAMatch(mToRow, mToCol)) {

				mChains.clear();
				FindMatches();
				RemoveMatches();
				FillHoles();

				mDoneAnimation.clear();
				mStatus = Game::ANIMATE_FALLING;
			}
			else {

				//its not a match, swap back
				mSwapBack = true;
				UpdateToSwap(mFromRow, mFromCol, mToRow, mToCol);
			}
			break;

		case Game::ANIMATE_FALLING:

			// Animate the falling gems
			AnimateGems(mFallingAnimation);
			if (mFallingAnimation.empty()) {
				// Ended the falling animation will refill the top and called
				// the new gems
				RefillTop();
				mDoneAnimation.clear();
				mStatus = Game::ANIMATE_NEW_GEMS;
			}
			break;

		case Game::ANIMATE_NEW_GEMS:

			//Animate the new gems
			AnimateGemsTop();
			if (mRefillingAnimation.empty()) {
				if (!mIsGoingToEnd) {
					mStatus = Game::END_PROCESSING;					
				}
				else {
					mStatus = Game::END;
				}
				

			}
			break;

		case Game::END_PROCESSING:

			mFromRow = 0;
			mToRow = 0;
			mFromCol = 0;
			mToCol = 0;
			mClickCoordX = 0.0f;
			mClickCoordY = 0.0f;
			mReleaseCoordX = 0.0f;
			mReleaseCoordY = 0.0f;

			if (/*mChains.size() >  0 &&*/ mFallingAnimation.empty() && mRefillingAnimation.empty()) {
				mChains.clear();
				HorizontalMatches(mGems);
				VerticalMatches(mGems);
				RemoveMatches();
				FillHoles();

				mStatus = Game::ANIMATE_FALLING;

			}
			
			break;

		case Game::MATCH_AFTER_PROCESSING:
			
			RemoveMatches();
			FillHoles();

			mDoneAnimation.clear();
			mStatus = Game::ANIMATE_FALLING;
			break;
		
		case Game::END:
			std::this_thread::sleep_for(std::chrono::seconds(5));
			mEngine.Quit();

			break;
		
		default:
		break;
	}

	// window rendering
	Render();

}

void Game::Init()
{

	srand((unsigned int)time(NULL));
	for (auto i = 0; i < Util::ROWS; ++i)
		for (auto j = 0; j < Util::COLS; ++j)
			mGems[i][j] = nullptr;

}

void Game::Load()
{
	// load gems
	auto row = 0;
	for (; row != Util::ROWS; ++row) {
		for (auto col = 0; col < Util::COLS; ++col) {
			GameGem * gem = GetNewGem(row,col);			
			mGems[row][col] = gem;
			std::cout << " [Gem] x= " << gem->GetPosition().mOrigX << " y = " << gem->GetPosition().mOrigY << " row = " << row << " col = " << row << std::endl;			
		}
	}

	mStart = chrono_clock::now();
}

void Game::Render()
{

	//Render board
	
	mEngine.Render(King::Engine::TEXTURE_BACKGROUND, 0.0f, 0.0f);

	for (auto row = 0; row < Util::ROWS; ++row) {
		for (auto col = 0; col < Util::COLS; ++col) {
			if (mGems[row][col] != nullptr) {
				mEngine.Render(mGems[row][col]->GetTexture(), mGems[row][col]->GetPosition().mOrigX, mGems[row][col]->GetPosition().mOrigY);
			}
		}
	}

	// Updat eTime and Score
	//(choose to use chrono instead of sdl_ticks mainly to keepup the code short)
	auto timer = Util::INITIAL_TIME - std::chrono::duration_cast<std::chrono::seconds>( (chrono_clock::now() - mStart)).count();
	timer = timer < 0 ? 0 : timer;
	
	auto displayedTimer = Util::TIMER + std::to_string(timer);
	auto displayedScore = Util::SCORE + std::to_string(mScore);
	
	mEngine.Write(displayedScore.c_str(), Util::POSX_SCORE, Util::POSY_SCORE);
	mEngine.Write(displayedTimer.c_str(), Util::POSX_TIMER, Util::POSY_TIMER);
	
	if (timer  == 0) {
		auto gameOver = (" GAME OVER! SCORE : " + std::to_string(mScore));
		mEngine.Write(gameOver.c_str(), 310.0f, 50.0f);
		if (mRefillingAnimation.empty() && mFallingAnimation.empty())
			mStatus = Game::END;
		else
			mIsGoingToEnd = true;
	}
}

void Game::Swap(short& fromRow, short& fromCol, short& toRow, short& toCol)
{
	//Swap all the values 
	GameGem from = *mGems[fromRow][fromCol];
	GameGem to = *mGems[toRow][toCol];

	std::swap(from, to);
	std::swap(fromRow, toRow);
	std::swap(fromCol, toCol);

	from.SetRow(toRow);
	to.SetRow(fromRow);
	from.SetCol(toCol);
	to.SetCol(fromCol);

	GameGem  g = from;

	*mGems[fromRow][fromCol] = to;
	*mGems[toRow][toCol] = g;

}

void Game::UpdateToSwap(short fromRow, short fromCol, short toRow, short toCol)
{

	// Update status and variables to swap gems
	SwapGem::GetInstance().SetStatus(SwapGem::SWAPPING);

	Position from = mGems[fromRow][fromCol]->GetPosition();
	Position to = mGems[toRow][toCol]->GetPosition();

	UpdateDestination(from, to);

	mGems[mFromRow][mFromCol]->SetPosition(from);
	mGems[mToRow][mToCol]->SetPosition(to);

	mStatus = Game::SWAP;
}

void Game::OnMouseButtonUp(float& releaseCoordX, float& releaseCoordY)
{
	// Get the release coordinates and check if it is a new gem touched/swapped
	releaseCoordX = mEngine.GetMouseX();
	releaseCoordY = mEngine.GetMouseY();
	
	GetCoordinatesFromClick(releaseCoordX, releaseCoordY, mToRow, mToCol);

	//Get the coordinates and adjust to the next object in case it was 
	// release too far from the original gem
	if ((mFromRow + 1) < mToRow)
		mToRow = mFromRow + 1;
	else if ((mFromRow - 1 )> mToRow)
		mToRow = mFromRow - 1;

	if ((mFromCol + 1) < mToCol)
		mToCol = mFromCol + 1;
	else if ((mFromCol - 1) > mToCol)
		mToCol = mFromCol - 1;
	
	// if is the same point pressed and release do nothing or a diagonal movement do nothing
	if ((mFromRow> 0 && mFromRow == mToRow && mFromCol == mToCol) ||
		
		mFromRow != mToRow && mToCol != mFromCol)	{
		SwapGem::GetInstance().SetStatus(SwapGem::Swapping::IDLE);
		mStatus = Game::IDLE;
		return;
	}

	UpdateToSwap(mFromRow, mFromCol, mToRow, mToCol);

	mStatus = Game::SWAP;
}

void Game::OnMouseButtonDown(float& clickCoordX, float& clickCoordY)
{
 
	// Check if its a valid clicked and if its not waiitng other status

	if (mWaitingMouseUp)
		return;

	clickCoordX = mEngine.GetMouseX();
	clickCoordY = mEngine.GetMouseY();
	if (IsValidGem(clickCoordX, clickCoordY) && SwapGem::GetInstance().SwapStatus() != SwapGem::Swapping::STARTED) {
		
		GetCoordinatesFromClick(clickCoordX, clickCoordY,mFromRow,mFromCol);
		SwapGem::GetInstance().SetStatus(SwapGem::Swapping::STARTED);
		mWaitingMouseUp = true;
		
	}

}

King::Engine::Texture Game::GetRandomGem(int row, int col)
{
	King::Engine::Texture texture;

	// Get the next gem and avoid creating chain on the process
	do {
		texture = static_cast<King::Engine::Texture>(1 + rand() % 5);

	} while ((col >= 2 &&
		mGems[row][col - 1]->GetTexture() == texture &&
		mGems[row][col - 2]->GetTexture() == texture)
		||
		(row >= 2 &&
			mGems[row - 1][col]->GetTexture() == texture &&
			mGems[row - 2][col]->GetTexture() == texture));

	return texture;
}

bool Game::IsValidGem(float& x, float& y)
{
	bool validCoords = IsValidCoordinates(Util::POSX_BOARD_INITIAL, Util::POSX_BOARD_END, x) && IsValidCoordinates(Util::POSY_BOARD_INITIAL, Util::POSY_BOARD_END, y);
	
	return validCoords;

}

bool Game::IsValidCoordinates(const float& initial, const float& end, float& coord)
{
	return (coord >= initial && coord <= end);

}

void Game::GetCoordinatesFromClick(const float coordX, const float coordY, short& row, short& col)
{
	row = ((short)(coordY - (short)Util::POSY_BOARD_INITIAL) / ((short)Util::GEM_HEIGHT + (short)Util::GEM_OFFSET_Y));
	col = ((short)(coordX - (short)Util::POSX_BOARD_INITIAL) / ((short)Util::GEM_WIDTH + (short)Util::GEM_OFFSET_X));

	assert(row <= Util::ROWS && col <= Util::COLS);
}

 GameGem * Game::GetNewGem(short row, short col)
{
	GameGem * gem = new GameGem();
	
	gem->SetTexture(GetRandomGem(row, col));	
	gem->SetRow(row);
	gem->SetCol(col);
	gem->SetPosition((Util::POSX_BOARD_INITIAL + col*Util::GEM_WIDTH + col*Util::GEM_OFFSET_X),
		(Util::POSY_BOARD_INITIAL + row*Util::GEM_HEIGHT + row*Util::GEM_OFFSET_Y));

	return gem;
}

bool Game::ItsAMatch(const short row, const short col)
{
	King::Engine::Texture texture = mGems[row][col]->GetTexture();
	
	bool horizontal = isHorizontalMatch(texture, row, col);
	bool vertical = isVerticalMatch(texture, row, col);

	return horizontal || vertical;
}

void Game::UpdateDestination(Position& from, Position& to)
{

	//Update the destination of each gem

	from.mDestX = to.mOrigX;
	from.mDestY = to.mOrigY;
	
	to.mDestX = from.mOrigX;
	to.mDestY = from.mOrigY;
}

void Game::FindMatches()
{
	HorizontalMatches();
	VerticalMatches();
}

bool Game::isHorizontalMatch(King::Engine::Texture texture, const short row, const short col)
{
	


	// Check for the match , while the gem is not a nullptr and its color is a match will find (increment) the chain
	// Check if the next is a same texture gem, if not stop the loop
	// if it is go until find at least 3 in a chain
	
	auto chain = 1;
	auto newCol = col - 1;
	for (; newCol >= 0 && mGems[row][newCol] != nullptr && mGems[row][newCol]->GetTexture() == texture; --newCol, ++chain);

	newCol = col + 1;
	for (; newCol < Util::COLS && mGems[row][newCol] != nullptr && mGems[row][newCol]->GetTexture() == texture; ++newCol, ++chain);
	
	return (chain >= Util::CHAIN_SIZE);

}

bool Game::isVerticalMatch(King::Engine::Texture texture, const short row, const short col)
{
	/*
		Check for the vertical match , while the gem is not a nullptr and its color is a match will find (increment) the chain
	*/

	auto chain = 1;

	auto newRow = row-1;	
	for (; newRow >= 0 && mGems[newRow][col] != nullptr && mGems[newRow][col]->GetTexture() == texture; ++chain, --newRow);

	newRow = row+1;
	for (; newRow < Util::ROWS && mGems[newRow][col] != nullptr && mGems[newRow][col]->GetTexture() == texture; ++chain, ++newRow);

	return (chain >= Util::CHAIN_SIZE);
}

void Game::HorizontalMatches()
{
	// scroll through the columns to see if finds at least 3 consecutive gems
	// The (col -2) is to ensure we're staying within valid col in the matrix
	
	for (auto row = 0; row < Util::ROWS; row++)	{
		for (auto col = 0; col < (Util::COLS - 2);)	{			
			
			if (mGems[row][col] != nullptr)	{

				//check if current gem is in a chain
				if (mGems[row][col + 1] != nullptr && 	mGems[row][col + 1]->GetTexture() == mGems[row][col]->GetTexture() &&
					mGems[row][col + 2] != nullptr &&	mGems[row][col + 2]->GetTexture() == mGems[row][col]->GetTexture()){

					// found a chain ( at least 3 ) add to the vector that will handle the disappearing
					
					ChainGem* chain = new ChainGem();

					auto chainSize = 0;
					do
					{
						chain->Push(mGems[row][col]);
						col++;
						chainSize++;
					} while (col < Util::COLS && mGems[row][col] != nullptr &&
						mGems[row][col]->GetTexture() == mGems[row][col]->GetTexture());

					UpdateScore(chainSize);
					mChains.push_back(chain);
				}
			}
			//update col
			col++;
		}
	}

}


void Game::HorizontalMatches(GameGem* const gems[Util::ROWS][Util::COLS])
{
	// scroll through the columns to see if finds at least 3 consecutive gems
	// The (col -2) is to ensure we're staying within valid col in the matrix

	for (auto row = 0; row < Util::ROWS; row++) {
		for (auto col = 0; col < (Util::COLS - 2);) {

			if (gems[row][col] != nullptr) {

				//check if current gem is in a chain
				if (gems[row][col + 1] != nullptr && gems[row][col + 1]->GetTexture() == gems[row][col]->GetTexture() &&
					gems[row][col + 2] != nullptr && gems[row][col + 2]->GetTexture() == gems[row][col]->GetTexture()) {

					// found a chain ( at least 3 ) add to the vector that will handle the disappearing
					ChainGem* chain = new ChainGem();

					auto chainSize = 0;
					do
					{
						chain->Push(gems[row][col]);
						col++;
						chainSize++;
					} while (col < Util::COLS && gems[row][col] != nullptr &&
						gems[row][col]->GetTexture() == gems[row][col]->GetTexture());

					UpdateScore(chainSize);
					mChains.push_back(chain);
				}
			}
			// Updpate col
			col++;
		}
	}

}



void Game::VerticalMatches()
{

	for (auto col = 0; col < Util::COLS; col++) {
		/* to avoid out of bound*/
		for (auto row = 0; row < (Util::ROWS - 2);) {
			if (mGems[row][col] != nullptr) {

				//check if current gem is in a chain
				if (mGems[row + 1][col] != nullptr && mGems[row + 1][col]->GetTexture() == mGems[row][col]->GetTexture() &&
					mGems[row + 2][col] != nullptr && mGems[row + 2][col]->GetTexture() == mGems[row][col]->GetTexture()) {
					
					ChainGem* chain =  new ChainGem();
					
					auto chainSize = 0;

					do
					{
						chain->Push(mGems[row][col]);
						row++;
						chainSize++;
					} while (row < Util::ROWS-1 && col < Util::COLS && mGems[row][col] != nullptr &&
						mGems[row][col]->GetTexture() == mGems[row][col]->GetTexture());


					//for (; row < Util::COLS && mGems[row][col] != nullptr
					//	&& mGems[row][col]->GetTexture() == texture; row++, ++chainSize) {
					//	chain->Push(mGems[row][col]);
					//}

					UpdateScore(chainSize);
					mChains.push_back(chain);
				}
			}
			row++;
		}
	}
}


void Game::VerticalMatches(GameGem* const gems[Util::COLS][Util::ROWS] )
{

	for (auto col = 0; col < Util::COLS; col++) {
		/* to avoid out of bound*/
		for (auto row = 0; row < (Util::ROWS - 2);) {
			if (gems[row][col] != nullptr) {

				//check if current gem is in a chain
				if (gems[row + 1][col] != nullptr && gems[row + 1][col]->GetTexture() == gems[row][col]->GetTexture() &&
					gems[row + 2][col] != nullptr && gems[row + 2][col]->GetTexture() == gems[row][col]->GetTexture()) {

					ChainGem* chain = new ChainGem();

					auto chainSize = 0;

					do
					{
						chain->Push(gems[row][col]);
						row++;
						chainSize++;
					} while (col < Util::COLS && gems[row][col] != nullptr &&
						gems[row][col]->GetTexture() == gems[row][col]->GetTexture());


					//for (; row < Util::COLS && mGems[row][col] != nullptr
					//	&& mGems[row][col]->GetTexture() == texture; row++, ++chainSize) {
					//	chain->Push(mGems[row][col]);
					//}

					UpdateScore(chainSize);
					mChains.push_back(chain);
				}
			}
			row++;
		}
	}
}



void Game::RemoveMatches()
{
	for each (auto itChain in mChains)
	{
		mFillHoles = true;
		for (auto itGem = itChain->GetChain()->begin(); itGem != itChain->GetChain()->end(); itGem++) {
			auto row = (*itGem)->GetRow();
			auto col = (*itGem)->GetCol();
			if (row >= 0 && row < Util::ROWS && col >= 0 && col < Util::COLS) {
				mGems[row][col] = nullptr;
			}
		}
	}

	if (!mChains.empty())
		mChains.clear();
}

void Game::FillHoles()
{
	if (mFillHoles) {
		
		// look for gem to fill , it will to the search bottom to up 
		for (auto col = 0; col < Util::COLS; col++){
			for (int row = (Util::ROWS - 1); row >= 0; row--){
				if (mGems[row][col] == nullptr)
				{
					// found a hole now checkup up if has another hole
					// invalidate the position and then push the gem down and add to be animated it later
					for (auto lookup = row - 1; lookup >= 0; --lookup)	{
						GameGem * gem = mGems[lookup][col];						
						
						if (gem != nullptr){
							gem->SetRow(row);
							mGems[row][col] =gem;
							mGems[lookup][col] = nullptr;
							mFallingAnimation.push_back(gem);
							break;
						}
					}
				}
			}
		}

		mFillHoles = false;
		//after fill the holes get new gems and refill the top
		mStatus = Game::REFILL_TOP;

	}
}

void Game::AnimateGems(std::vector<GameGem*>& animate)
{
	for (size_t i = 0; i < animate.size(); i++ ) {

		GameGem *gem = animate[i];
		auto newY = Util::POSY_BOARD_INITIAL + (gem->GetRow() * Util::GEM_HEIGHT) + (Util::GEM_OFFSET_Y * gem->GetRow());
		Position pos = gem->GetPosition();
		pos.mDestY = newY;

		float val = 0;

		if (pos.mOrigY != pos.mDestY) {
			pos.mOrigY += 5.0f;
			gem->SetPosition(pos);
		}
		else {
			animate.erase(animate.begin() + i);
		}
		
	}

}

void Game::RefillTop()
{

	//Create new gems at the top, in the initial Y

	for (auto col = 0; col < Util::COLS; ++col){
		for (auto row = 0; row < Util::ROWS && mGems[row][col] == nullptr; ++row)
		{
			GameGem * gem = GetNewGem(row, col);
			Position pos;
			pos = gem->GetPosition();
			pos.mOrigY = Util::POSY_BOARD_INITIAL;
			gem->SetPosition(pos);
			mGems[row][col] = gem;

			mRefillingAnimation.push_back(gem);
		}
	}

}

void Game::UpdateScore(short chainSize)
{
	if (chainSize == Util::SMALL_CHAIN)
		mScore += Util::SCORE_SMALL_CHAIN;
	else if (chainSize <= Util::MEDIUM_CHAIN)
		mScore += Util::SCORE_MEDIUM_CHAIN;
	else if (chainSize <= Util::MEDIUM_CHAIN)
		mScore += Util::SCORE_LARGE_CHAIN;
}

void Game::AnimateGemsTop()
{
	for (size_t i = 0; i < mRefillingAnimation.size(); i++)
	{
		GameGem *gem = mRefillingAnimation[i];
		auto newY = Util::POSY_BOARD_INITIAL + (gem->GetRow() * Util::GEM_HEIGHT) + (Util::GEM_OFFSET_Y * gem->GetRow());

		Position pos = gem->GetPosition();
		pos.mDestY = newY;
		float val = 0;

		if (pos.mOrigY == pos.mDestY) {
			mRefillingAnimation.erase(mRefillingAnimation.begin() + i);
		}
		else {
			pos.mOrigY += 4.0f;
			gem->SetPosition(pos);
		}
	}

}


