#include "PathStep.h"

using namespace cocos2d;

PathStep::PathStep()
	: m_index(Point::ZERO)
	, m_gScore(0)
	, m_hScore(0)
	, m_parent(nullptr)
{

}

PathStep::~PathStep()
{

}

PathStep * PathStep::createWithIndex(const cocos2d::Point &index)
{
	PathStep *pRet = new PathStep();
	if (pRet && pRet->initWithIndex(index))
	{
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return nullptr;
	}
}

bool PathStep::initWithIndex(const cocos2d::Point &index)
{
	this->setIndex(index);
	return true;
}

int PathStep::getFScore() const
{
	return this->getGScore() + this->getHScore();
}

bool PathStep::isEqual(const PathStep *other) const
{
	return this->getIndex() == other->getIndex();
}
