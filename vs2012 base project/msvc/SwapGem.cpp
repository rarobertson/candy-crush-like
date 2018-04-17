#include "SwapGem.h"
#include "Util.h"



//SwapGem::SwapGem():/*
//	mFromGem(nullptr),
//	mToGem(nullptr),*/
//	mEndSwap(false),
//	mStatus(Swapping::IDLE)
//{
//}

//SwapGem::SwapGem(std::shared_ptr<GameGem> from, std::shared_ptr<GameGem> to)
//{
//	//mFromGem = from;
//	//mToGem = to;
//}

SwapGem::~SwapGem()
{
}


void SwapGem::UpdateSwap(Position& fromPosition, Position& toPosition)
{
	mStatus = Swapping::SWAPPING;
	
	//Swap the coords value
	//
	Swap(fromPosition.mOrigX, toPosition.mOrigX, fromPosition.mDestX, toPosition.mDestX, fromPosition.mFade);
	Swap(fromPosition.mOrigY, toPosition.mOrigY, fromPosition.mDestY, toPosition.mDestY, fromPosition.mFade);

}

bool SwapGem::IsSwapped()
{
	return (mStatus == Swapping::END);
}

SwapGem::Swapping SwapGem::SwapStatus() const
{
	return mStatus;
}

void SwapGem::SetStatus(SwapGem::Swapping val)
{
	mStatus = val;
}

void SwapGem::Swap(float& from, float& to, float destFrom, float destTo, float fade)
{
	
	auto isValidDirection = false;
	
	/*update the cords value so the texture can be displayed as swapped
		Check what direction it should do it and update it 
	*/

	if (from != destFrom && destTo != to) {
		UpdatePosition(from, to, fade,destFrom,destTo);
		isValidDirection = true;
	}
	
	//if it was a valid direction ( right, left, up, down ) and it reaches it ends stop swapping
	if (isValidDirection && from == destFrom && to == destTo) {
		mEndSwap = true;
		mStatus = Swapping::END;
	}
}

void SwapGem::UpdatePosition(float& posA, float& posB, float fade, float posDestA, float postDestB)
{
	// Update the gem coords according to which direction is swapping

	if(posA != posDestA)
		posA = posA > posDestA ? posA - fade : posA + fade;

	if(posB != postDestB)
		posB = posB > postDestB ? posB - fade : posB + fade;
}

