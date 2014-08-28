#include "Player.h"
#include "PathStep.h"
#include "TestLayer.h"

using namespace cocos2d;

Player::Player()
	: m_delegateLayer(nullptr)
{

}

Player::~Player()
{

}

bool Player::init()
{
	if (!Sprite::initWithFile("player.png"))
	{
		return false;
	}


	return true;
}

void Player::stopFindPathAndResetPosition()
{
	this->stopAllActions();
	this->setPositionForIndex(m_delegateLayer->tileIndexForPosition(this->getPosition()));
	clearStepTable(m_openTable);
	clearStepTable(m_closedTable);
	m_delegateLayer->unDrawPath();
}

void Player::clearStepTable(std::vector<PathStep*> &stepTable)
{
	for (std::vector<PathStep*>::iterator it=stepTable.begin(); it!=stepTable.end(); it++)
	{
		PathStep* step = *it;
		if (step)
		{
			delete step;
			step = NULL;
		}
	}
	stepTable.clear();
}

int Player::getIndexStepInTable(const std::vector<PathStep *> &steps, const PathStep *step)
{
	for (int i = 0; i < steps.size(); i++)
	{
		if (steps.at(i)->isEqual(step))
		{
			return i;
		}
	}
	return -1;
}

void Player::moveToward(const cocos2d::Point &targetPosition)
{
	this->stopFindPathAndResetPosition();

	Point fromTileIndex = m_delegateLayer->tileIndexForPosition(this->getPosition());
	Point toTileIndex = m_delegateLayer->tileIndexForPosition(targetPosition);
	if (fromTileIndex == toTileIndex)
	{
		CCLOG("You're already move to target");
		return;
	}
	if (!m_delegateLayer->isValidTileAtIndex(toTileIndex) || m_delegateLayer->isWallTileAtIndex(toTileIndex))
	{
		CCLOG("This tile is not valid");
		return;
	}

	//首先，添加方块坐标到open列表
	this->insertInOpenSteps(PathStep::createWithIndex(fromTileIndex));
	do{
		//得到最小的F值步骤,因为是有序列表，第一个步骤总是最小的F值
		PathStep *currentStep = m_openTable.at(0);
		//添加当前步骤到closed列表
		m_closedTable.push_back(currentStep);
		//将它从open列表里面移除
		m_openTable.erase(m_openTable.begin());
		//如果当前步骤是目标方块坐标，那么就完成了
		if (currentStep->getIndex() == toTileIndex)
		{
			this->constructPathAndStartMoveFromStep(currentStep);
			break;
		}

		//得到当前步骤的相邻方块坐标
		std::vector<Point> adjacentSteps = m_delegateLayer->walkableAdjacentTilesIndexForTileIndex(currentStep->getIndex());
		for (int i = 0; i < adjacentSteps.size(); i++)
		{
			PathStep* step = PathStep::createWithIndex(adjacentSteps[i]);
			//检查步骤是不是已经在closed列表
			if (this->getIndexStepInTable(m_closedTable, step) != -1)
			{
				delete step;
				continue;
			}
			//计算从当前步骤到此步骤的成本
			int moveCost = this->costFromStepToAdjacentStep(currentStep, step);
			//检查此步骤是否已经在open列表
			int index = this->getIndexStepInTable(m_openTable, step);
			//不在open列表，添加它
			if (index == -1)
			{
				//设置当前步骤作为上一步操作
				step->setParent(currentStep);
				//G值等同于上一步的G值 + 从上一步到这里的成本
				step->setGScore(currentStep->getGScore() + moveCost);
				//H值即是从此步骤到目标方块坐标的移动量估算值
				step->setHScore(this->computeHScoreFromIndexToIndex(step->getIndex(), toTileIndex));
				//按序添加到open列表
				this->insertInOpenSteps(step);
			}
			else
			{
				//获取旧的步骤，其值已经计算过
				step = m_openTable.at(index);
				//检查G值是否低于当前步骤到此步骤的值
				if ((currentStep->getGScore() + moveCost) < step->getGScore())
				{
					//G值等同于上一步的G值 + 从上一步到这里的成本
					step->setGScore(currentStep->getGScore() + moveCost);
					//因为G值改变了，F值也会跟着改变,所以为了保持open列表有序，需要将此步骤移除，再重新按序插入
					m_openTable.erase(m_openTable.begin() + index);
					// 重新按序插入
					this->insertInOpenSteps(step);
				}
			}
		}
	} while (m_openTable.size() > 0);

	if (m_stepPath.empty())
	{
		CCLOG("hit wall");
	}
}

void Player::insertInOpenSteps(PathStep *step)
{
	int stepFScore = step->getFScore();
	int count = m_openTable.size();
	int i = 0;
	for (; i < count; i++)
	{
		if (stepFScore <= m_openTable.at(i)->getFScore())
		{
			break;
		}
	}
	m_openTable.insert(m_openTable.begin() + i, step);
}

int Player::computeHScoreFromIndexToIndex(const cocos2d::Point& fromIndex, const cocos2d::Point &toIndex)
{
	return abs(toIndex.x - fromIndex.x) + abs(toIndex.y - fromIndex.y);
}

int Player::costFromStepToAdjacentStep(const PathStep *fromStep, const PathStep *toStep)
{
	return 1;
}

void Player::constructPathAndStartMoveFromStep(PathStep *step)
{
	std::vector<Point> vecIndex;

	m_stepPath.clear();
	do{
		//起始位置不要进行添加
		if (step->getParent())
		{
			//总是插入到索引0的位置，以便反转路径
			m_stepPath.insert(m_stepPath.begin(), step);

			vecIndex.push_back(step->getIndex());
		}
		step = step->getParent();   //倒退
	} while (step);                 //直到没有上一步

	vecIndex.push_back(m_delegateLayer->tileIndexForPosition(this->getPosition()));
	m_delegateLayer->drawPath(vecIndex);

	this->popStepAndMove();
}

void Player::popStepAndMove()
{
	if (m_stepPath.size() == 0)
	{
		return;
	}

	//得到下一步移动的步骤
	PathStep *s = m_stepPath.at(0);
	Point indexMoveTo = s->getIndex();

	//准备动作和回调
	MoveTo *moveTo = MoveTo::create(0.2f, m_delegateLayer->positionForTileIndex(indexMoveTo));
	CallFunc *moveCallback = CallFunc::create(CC_CALLBACK_0(Player::popStepAndMove, this));
	//移除步骤
	m_stepPath.erase(m_stepPath.begin());
	//运行动作
	Sequence *moveSequence = Sequence::create(moveTo, moveCallback, nullptr);
	this->runAction(moveSequence);
}

void Player::setPositionForIndex(cocos2d::Point tileIndex)
{
	Point pos = m_delegateLayer->positionForTileIndex(tileIndex);
	this->setPosition(pos);
}
