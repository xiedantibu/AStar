#include "TestLayer.h"
#include "Player.h"

using namespace cocos2d;


TestLayer::TestLayer()
	: m_tileMap(nullptr)
	, m_background(nullptr)
	, m_wall(nullptr)
	, m_isTouchPlayer(false)
	, m_tileIndexTouchBegan(Point(0, 19))
	, m_isDrawPath(false)
{

}

TestLayer::~TestLayer()
{

}

Scene* TestLayer::createScene()
{
    auto scene = Scene::create();
    auto layer = TestLayer::create();
    scene->addChild(layer);
    return scene;
}

bool TestLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
	m_tileMap = TMXTiledMap::create("map.tmx");
	this->addChild(m_tileMap);

	m_background = m_tileMap->getLayer("background");
	m_wall = m_tileMap->getLayer("wall");

    m_playerRole = Player::create();
	m_playerRole->setDelegateLayer(this);
	m_tileMap->addChild(m_playerRole);

	Point tileIndex(0, 19);
	Point pos = this->positionForTileIndex(tileIndex);
	m_playerRole->setPosition(pos);


	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);

	listener->onTouchBegan = CC_CALLBACK_2(TestLayer::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(TestLayer::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(TestLayer::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

bool TestLayer::onTouchBegan(Touch* touch, Event* event)
{
	Point touchLocation = m_tileMap->convertTouchToNodeSpace(touch);
	Point tileIndex = this->tileIndexForPosition(touchLocation);
	m_tileIndexTouchBegan = tileIndex;
	m_isTouchPlayer = this->isPlayerInTouchTile(touchLocation);
	if (m_isTouchPlayer)
	{
		m_playerRole->stopFindPathAndResetPosition();
	}
	return true;
}

void TestLayer::onTouchMoved(Touch* touch, Event* event)
{
	if (m_isTouchPlayer)
	{
		Point touchLocation = m_tileMap->convertTouchToNodeSpace(touch);
		Point tileIndex = this->tileIndexForPosition(touchLocation);
		m_playerRole->setPositionForIndex(tileIndex);
	}
}

void TestLayer::onTouchEnded(Touch* touch, Event* event)
{
	Point touchLocation = m_tileMap->convertTouchToNodeSpace(touch);
	Point tileIndex = this->tileIndexForPosition(touchLocation);

	if (m_isTouchPlayer)
	{
		if (!isValidTileAtIndex(tileIndex) || isWallTileAtIndex(tileIndex))
		{
			m_playerRole->setPositionForIndex(m_tileIndexTouchBegan);
		}
		else
		{
			m_playerRole->setPositionForIndex(tileIndex);
		}
	}
	else
	{
		m_playerRole->moveToward(touchLocation);
	}
}

bool TestLayer::isPlayerInTouchTile(cocos2d::Point position)
{
	Point positionPlayer = m_playerRole->getPosition();
	Point indexPlayer = this->tileIndexForPosition(positionPlayer);
	Point indexTouch = this->tileIndexForPosition(position);
	return indexPlayer.equals(indexTouch);
}

bool TestLayer::isPropAtTileIndexForLayer(const char *prop, const cocos2d::Point &tileIndex, cocos2d::TMXLayer *layer)
{
	if (!this->isValidTileAtIndex(tileIndex))
	{
		return false;
	}

	int gid = layer->getTileGIDAt(tileIndex);
	Value properties = m_tileMap->getPropertiesForGID(gid);
	if (properties.isNull())
	{
		return false;
	}
	return properties.asValueMap().find(prop) != properties.asValueMap().end();
}

bool TestLayer::isWallTileAtIndex(const cocos2d::Point &tileIndex)
{
	return this->isPropAtTileIndexForLayer("wall", tileIndex, m_wall);
}

bool TestLayer::isValidTileAtIndex(const cocos2d::Point &tileIndex)
{
	if (tileIndex.x < 0 ||
		tileIndex.y < 0 ||
		tileIndex.x >= m_tileMap->getMapSize().width ||
		tileIndex.y >= m_tileMap->getMapSize().height)
	{
		return false;
	}
	else
	{
		return true;
	}
}

cocos2d::Point TestLayer::tileIndexForPosition(const cocos2d::Point &position)
{
	int x = position.x / m_tileMap->getTileSize().width;
	int y = m_tileMap->getMapSize().height - position.y / m_tileMap->getTileSize().height;
	return Point(x, y);
}

cocos2d::Point TestLayer::positionForTileIndex(const cocos2d::Point &tileIndex)
{
	int posX = (tileIndex.x * m_tileMap->getTileSize().width) + m_tileMap->getTileSize().width / 2;
	int posY = (m_tileMap->getMapSize().height - tileIndex.y) * m_tileMap->getTileSize().height - m_tileMap->getTileSize().height / 2;
	return Point(posX, posY);
}

std::vector<cocos2d::Point> TestLayer::walkableAdjacentTilesIndexForTileIndex(const cocos2d::Point &tileIndex)
{
	std::vector<cocos2d::Point> ret;
	//ио
	Point p_up(tileIndex.x, tileIndex.y - 1);
	if (this->isValidTileAtIndex(p_up) && !this->isWallTileAtIndex(p_up))
	{
		ret.push_back(p_up);
	}
	//об
	Point p_down(tileIndex.x, tileIndex.y + 1);
	if (this->isValidTileAtIndex(p_down) && !this->isWallTileAtIndex(p_down))
	{
		ret.push_back(p_down);
	}
	//вС
	Point p_left(tileIndex.x - 1, tileIndex.y);
	if (this->isValidTileAtIndex(p_left) && !this->isWallTileAtIndex(p_left))
	{
		ret.push_back(p_left);
	}

	//ср
	Point p_right(tileIndex.x + 1, tileIndex.y);
	if (this->isValidTileAtIndex(p_right) && !this->isWallTileAtIndex(p_right))
	{
		ret.push_back(p_right);
	}
	return ret;
}

void TestLayer::unDrawPath()
{
	m_isDrawPath = false;
	m_pathStep.clear();
}

void TestLayer::drawPath(std::vector<Point> pathStep)
{
	m_pathStep = pathStep;
	m_isDrawPath = true;
}

void TestLayer::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
	_customCommand.init(_globalZOrder+1);
	_customCommand.func = CC_CALLBACK_0(TestLayer::onDraw, this, transform, flags);
	renderer->addCommand(&_customCommand);
}

void TestLayer::onDraw(const Mat4 &transform, uint32_t flags)
{
	if (m_isDrawPath && m_pathStep.size() > 1)
	{
		Point pos = this->positionForTileIndex(m_pathStep[0]);

		for (unsigned int i=1; i<m_pathStep.size(); i++)
		{
			Point curPos = this->positionForTileIndex(m_pathStep[i]);

			CHECK_GL_ERROR_DEBUG();
			glLineWidth( 2.0f );
			DrawPrimitives::setDrawColor4B(0,0,255,255);
			DrawPrimitives::drawLine(pos, curPos);
			pos = curPos;
		}
	}

	glLineWidth(1);
	DrawPrimitives::setDrawColor4B(255,255,255,255);
	DrawPrimitives::setPointSize(1);
}






















