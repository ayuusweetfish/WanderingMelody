#include "AppDelegate.h"
#include "Global.h"
#include "scenes/Startup.h"
#include "scenes/Select.h"
#include "scenes/Gameplay.h"
#include "audio/AudioOutput.h"
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

    AudioOutput::createInstance();
    AudioOutput::getInstance()->startPlayback();

    printf("%08x\n", COCOS2D_VERSION);
    Controller::startDiscoveryController();

    auto scene = Select::createScene();
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
