#include "AppDelegate.h"
#include "Global.h"
#include "scenes/Startup.h"
#include "scenes/Gameplay.h"
using namespace cocos2d;

bool AppDelegate::applicationDidFinishLaunching()
{
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if (!glview) {
        glview = GLViewImpl::createWithRect("Wandering Melody", Rect(0, 0, WIN_W, WIN_H));
        director->setOpenGLView(glview);
    }
    director->setDisplayStats(true);
    director->setAnimationInterval(1.0 / 60);

    auto scene = Gameplay::createScene();
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
