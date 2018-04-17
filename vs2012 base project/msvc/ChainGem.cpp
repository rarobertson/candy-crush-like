#include "ChainGem.h"



ChainGem::ChainGem()
{
	mChain = std::make_shared<std::vector<GameGem *>>();
}


ChainGem::~ChainGem()
{
	mChain->clear();

}

void ChainGem::Push(GameGem* const gem)
{
	mChain->push_back(gem);
}

std::shared_ptr<std::vector<GameGem*>> ChainGem::GetChain() const
{
	return mChain;
}
