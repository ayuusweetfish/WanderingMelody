#include "AppDelegate.h"
#include "Global.h"
#include "scenes/Startup.h"
using namespace cocos2d;

bool AppDelegate::applicationDidFinishLaunching()
{
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if (!glview) {
#if IS_ON_PC
        glview = GLViewImpl::createWithRect("Wandering Melody", Rect(0, 0, 960, 540));
#else
        glview = GLViewImpl::createWithRect("Wandering Melody", Rect(0, 0, 640, 360));
#endif
        director->setOpenGLView(glview);
    }
    director->setDisplayStats(true);
    director->setAnimationInterval(1.0 / 60);

    auto scene = Startup::createScene();
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
