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

	//���ȣ���ӷ������굽open�б�
	this->insertInOpenSteps(PathStep::createWithIndex(fromTileIndex));
	do{
		//�õ���С��Fֵ����,��Ϊ�������б���һ������������С��Fֵ
		PathStep *currentStep = m_openTable.at(0);
		//��ӵ�ǰ���赽closed�б�
		m_closedTable.push_back(currentStep);
		//������open�б������Ƴ�
		m_openTable.erase(m_openTable.begin());
		//�����ǰ������Ŀ�귽�����꣬��ô�������
		if (currentStep->getIndex() == toTileIndex)
		{
			this->constructPathAndStartMoveFromStep(currentStep);
			break;
		}

		//�õ���ǰ��������ڷ�������
		std::vector<Point> adjacentSteps = m_delegateLayer->walkableAdjacentTilesIndexForTileIndex(currentStep->getIndex());
		for (int i = 0; i < adjacentSteps.size(); i++)
		{
			PathStep* step = PathStep::createWithIndex(adjacentSteps[i]);
			//��鲽���ǲ����Ѿ���closed�б�
			if (this->getIndexStepInTable(m_closedTable, step) != -1)
			{
				delete step;
				continue;
			}
			//����ӵ�ǰ���赽�˲���ĳɱ�
			int moveCost = this->costFromStepToAdjacentStep(currentStep, step);
			//���˲����Ƿ��Ѿ���open�б�
			int index = this->getIndexStepInTable(m_openTable, step);
			//����open�б������
			if (index == -1)
			{
				//���õ�ǰ������Ϊ��һ������
				step->setParent(currentStep);
				//Gֵ��ͬ����һ����Gֵ + ����һ��������ĳɱ�
				step->setGScore(currentStep->getGScore() + moveCost);
				//Hֵ���ǴӴ˲��赽Ŀ�귽��������ƶ�������ֵ
				step->setHScore(this->computeHScoreFromIndexToIndex(step->getIndex(), toTileIndex));
				//������ӵ�open�б�
				this->insertInOpenSteps(step);
			}
			else
			{
				//��ȡ�ɵĲ��裬��ֵ�Ѿ������
				step = m_openTable.at(index);
				//���Gֵ�Ƿ���ڵ�ǰ���赽�˲����ֵ
				if ((currentStep->getGScore() + moveCost) < step->getGScore())
				{
					//Gֵ��ͬ����һ����Gֵ + ����һ��������ĳɱ�
					step->setGScore(currentStep->getGScore() + moveCost);
					//��ΪGֵ�ı��ˣ�FֵҲ����Ÿı�,����Ϊ�˱���open�б�������Ҫ���˲����Ƴ��������°������
					m_openTable.erase(m_openTable.begin() + index);
					// ���°������
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
		//��ʼλ�ò�Ҫ�������
		if (step->getParent())
		{
			//���ǲ��뵽����0��λ�ã��Ա㷴ת·��
			m_stepPath.insert(m_stepPath.begin(), step);

			vecIndex.push_back(step->getIndex());
		}
		step = step->getParent();   //����
	} while (step);                 //ֱ��û����һ��

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

	//�õ���һ���ƶ��Ĳ���
	PathStep *s = m_stepPath.at(0);
	Point indexMoveTo = s->getIndex();

	//׼�������ͻص�
	MoveTo *moveTo = MoveTo::create(0.2f, m_delegateLayer->positionForTileIndex(indexMoveTo));
	CallFunc *moveCallback = CallFunc::create(CC_CALLBACK_0(Player::popStepAndMove, this));
	//�Ƴ�����
	m_stepPath.erase(m_stepPath.begin());
	//���ж���
	Sequence *moveSequence = Sequence::create(moveTo, moveCallback, nullptr);
	this->runAction(moveSequence);
}

void Player::setPositionForIndex(cocos2d::Point tileIndex)
{
	Point pos = m_delegateLayer->positionForTileIndex(tileIndex);
	this->setPosition(pos);
}
