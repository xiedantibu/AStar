#include "AppDelegate.h"
#include "TestLayer.h"

USING_NS_CC;

AppDelegate::AppDelegate()
{

}

AppDelegate::~AppDelegate() 
{

}

bool AppDelegate::applicationDidFinishLaunching()
{
	auto director = Director::getInstance();
	auto glview = director->getOpenGLView();
	if(!glview)
	{
		glview = GLView::createWithRect("AStar", Rect(0, 0, 960, 640), 1);
		director->setOpenGLView(glview);
	}
	glview->setDesignResolutionSize(960, 640, ResolutionPolicy::FIXED_HEIGHT);
	director->setDisplayStats(false);
	director->setAnimationInterval(1.0 / 60);

    auto scene = TestLayer::createScene();
    director->runWithScene(scene);

    return true;
}

void AppDelegate::applicationDidEnterBackground()
{
    Director::getInstance()->stopAnimation();
}

void AppDelegate::applicationWillEnterForeground()
{
    Director::getInstance()->startAnimation();
}
