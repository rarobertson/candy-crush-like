#pragma once

#include <memory>
#include "GameGem.h"

/*
	Class that swap the gem on the window and update their informaation 
	Its a singleton, in case more levels use it ( to do )
*/

class SwapGem
{
public:

	// Swapping status
	enum Swapping {
		IDLE,
		STARTED,
		SWAPPING,
		END
	};

	/*std::shared_ptr<GameGem> mFromGem;
	std::shared_ptr<GameGem> mToGem;*/

	static SwapGem& GetInstance()
	{
		static SwapGem SwapGem;
		return SwapGem;
	}
	SwapGem(SwapGem const&) = delete;
	void operator=(SwapGem const&) = delete;

	~SwapGem();
	
	void UpdateSwap(Position& fromPosition, Position& toPosition);

	bool IsSwapped();

	SwapGem::Swapping SwapStatus() const;
	void SetStatus(SwapGem::Swapping val);

	// Swap the coords betweem the selected gems
	void Swap(float& from, float& to, float destFrom, float destTo, float fade);

	// Update the position of the coords
	void UpdatePosition(float& posA, float& posB, float fade, float posDestA, float postDestB);


private:
	SwapGem() {};

	// storage the event of end the swap
	bool mEndSwap;

	// status of swapping
	Swapping mStatus;

};

