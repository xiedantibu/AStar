#pragma once

#include "cocos2d.h"
#include <vector>

class Player;


class TestLayer : public cocos2d::Layer
{
public:
	TestLayer();

	virtual ~TestLayer();

    static cocos2d::Scene* createScene();

    virtual bool init();

    CREATE_FUNC(TestLayer);

	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

	bool isPropAtTileIndexForLayer(const char *prop, const cocos2d::Point &tileIndex, cocos2d::TMXLayer *layer);

	bool isWallTileAtIndex(const cocos2d::Point &tileIndex);

	bool isValidTileAtIndex(const cocos2d::Point &tileIndex);

	cocos2d::Point tileIndexForPosition(const cocos2d::Point &position);

	cocos2d::Point positionForTileIndex(const cocos2d::Point &tileIndex);

	std::vector<cocos2d::Point> walkableAdjacentTilesIndexForTileIndex(const cocos2d::Point &tileIndex);

	//
	void unDrawPath();
	void drawPath(std::vector<cocos2d::Point> pathStep);
	virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;

private:
	bool isPlayerInTouchTile(cocos2d::Point position);

	void onDraw(const cocos2d::Mat4 &transform, uint32_t flags);
	cocos2d::CustomCommand _customCommand;

public:
	CC_SYNTHESIZE_READONLY(cocos2d::TMXTiledMap*, m_tileMap, TileMap);
	CC_SYNTHESIZE_READONLY(cocos2d::TMXLayer*, m_background, Background);
	CC_SYNTHESIZE_READONLY(cocos2d::TMXLayer*, m_wall, Wall);
	CC_SYNTHESIZE_READONLY(Player*, m_playerRole, PlayerRole);
	CC_SYNTHESIZE_READONLY(bool, m_isTouchPlayer, IsTouchPlayer);
	CC_SYNTHESIZE_READONLY(cocos2d::Point, m_tileIndexTouchBegan, TileIndexTouchBegan);

	CC_SYNTHESIZE_READONLY(bool, m_isDrawPath, IsDrawPath);
	CC_SYNTHESIZE_READONLY(std::vector<cocos2d::Point>, m_pathStep, PathStep);
};

