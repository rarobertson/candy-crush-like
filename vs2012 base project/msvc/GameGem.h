#pragma once

#include <king/Engine.h>
#include <King/SdlWindow.h>


// Struct that holds the gem posiion
struct Position {
	
	float mOrigX;
	float mOrigY;
	float mDestX;
	float mDestY;
	float mFade;

	Position() : mOrigX(0.0f), mOrigY(0.0f), mDestX(0.0f), mDestY(0.0f), mFade(0.0f) {};
};

/*
	Class that holds the information of the gem that are displayed
	position, row , col and the color of the gem
*/

class GameGem
{
public:
	
	GameGem();
	GameGem(const GameGem& source);
	GameGem(King::Engine::Texture gem, float x, float y);

	//GameGem& GameGem::operator= (const GameGem &source)
	//{
	//	*this = source;
	//	//mToGem = source.mFromGem;
	//	return *this;
	//}
	~GameGem();

	King::Engine::Texture GetTexture() const;	
	void SetTexture(King::Engine::Texture val);
	
	unsigned short GetRow() const;
	void SetRow(unsigned short val);
	
	unsigned short GetCol() const;
	void SetCol(unsigned short val);

	Position GetPosition();
	void SetPosition(Position val);
	
	void SetPosition(float x, float y, float fade = 5.0f);
	void SetDestination(float x, float y);

private:

	unsigned short mRow;
	unsigned short mCol;

	Position mPosition;

	King::Engine::Texture mTexture;

};

