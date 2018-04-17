#pragma once

#include <vector>
#include <memory>
#include "GameGem.h"

class ChainGem
{
public:

	ChainGem();
	~ChainGem();

	//Add one gem to the chain
	void Push(GameGem* const gem);

	std::shared_ptr<std::vector<GameGem*>> GetChain() const;

private:
	
	std::shared_ptr<std::vector<GameGem*>> mChain;
};
