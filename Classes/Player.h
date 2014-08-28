#pragma once

#include "cocos2d.h"
#include <vector>

class TestLayer;
class PathStep;


class Player : public cocos2d::Sprite
{
public:
	Player();

	virtual ~Player();

	virtual bool init();

	CREATE_FUNC(Player);

	void moveToward(const cocos2d::Point &targetPosition);

	void setPositionForIndex(cocos2d::Point tileIndex);

	void stopFindPathAndResetPosition();

private:

	void clearStepTable(std::vector<PathStep*> &stepTable);

	void insertInOpenSteps(PathStep *step);

	int getIndexStepInTable(const std::vector<PathStep *> &steps, const PathStep *step);

	int computeHScoreFromIndexToIndex(const cocos2d::Point& fromIndex, const cocos2d::Point &toIndex);

	int costFromStepToAdjacentStep(const PathStep *fromStep, const PathStep *toStep);

	void constructPathAndStartMoveFromStep(PathStep *step);

	void popStepAndMove();

public:

	CC_SYNTHESIZE(TestLayer*, m_delegateLayer, DelegateLayer);

private:

	std::vector<PathStep*> m_stepPath;
	std::vector<PathStep*> m_openTable;
	std::vector<PathStep*> m_closedTable;
};
