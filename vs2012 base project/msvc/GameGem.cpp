#include "GameGem.h"

GameGem::GameGem() {
	mPosition = Position();
}

GameGem::GameGem(const GameGem & source)
{
	*this = source;
}

GameGem::GameGem(King::Engine::Texture texture, float x, float y):	mTexture(texture) 
{
	mPosition.mOrigX = x;
	mPosition.mOrigY = y;
}


GameGem::~GameGem()
{
}

King::Engine::Texture GameGem::GetTexture() const {
	return mTexture;
}

void GameGem::SetTexture(King::Engine::Texture val)
{
	mTexture = val;
}

unsigned short GameGem::GetRow() const
{
	return mRow;
}

void GameGem::SetRow(unsigned short val)
{
	mRow = val;
}

unsigned short GameGem::GetCol() const
{
	return mCol;
}

void GameGem::SetCol(unsigned short val)
{
	mCol = val;
}

Position GameGem::GetPosition()  {
	return mPosition;
}

void GameGem::SetPosition(Position val)
{
	mPosition = val;
}

void GameGem::SetPosition(float x, float y, float fade)
{
	mPosition.mOrigX = x;
	mPosition.mOrigY = y;
	mPosition.mFade =fade;
}

void GameGem::SetDestination(float x, float y)
{
	mPosition.mDestX = x;
	mPosition.mDestY = y;
}

