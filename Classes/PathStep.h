#pragma once

#include "cocos2d.h"



class PathStep
{
public:
	PathStep();

	virtual ~PathStep();

	static PathStep *createWithIndex(const cocos2d::Point &index);

	bool initWithIndex(const cocos2d::Point &index);

	int getFScore() const;

	bool isEqual(const PathStep *other) const;

public:
	CC_SYNTHESIZE(cocos2d::Point, m_index, Index);
	CC_SYNTHESIZE(int, m_gScore, GScore);
	CC_SYNTHESIZE(int, m_hScore, HScore);
	CC_SYNTHESIZE(PathStep*, m_parent, Parent);
};