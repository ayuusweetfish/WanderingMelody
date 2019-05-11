#include "AppDelegate.h"
#include "Global.h"
#include "scenes/Startup.h"
#include "scenes/Gameplay.h"
#include "audio/AudioOutput.h"
using namespace cocos2d;

static int phase = 0;

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

    AudioOutput::createInstance();
    AudioOutput::getInstance()->startPlayback();

    AudioOutput::getInstance()->registerCallback([] (float *outbuf, int nframes) {
        for (int i = 0; i < nframes * 2; i++)
            outbuf[i] += sin((phase + i / 2) * M_PI / 100) * 0.2;
    });
    AudioOutput::getInstance()->registerCallback([] (float *outbuf, int nframes) {
        for (int i = 0; i < nframes * 2; i++)
            outbuf[i] += sin((phase + i / 2) * M_PI / 150) * 0.2;
        phase += nframes;
    });

    return true;
}

void AppDelegate::applicationDidEnterBackground()
{
    Director::getInstance()->stopAnimation();
    AudioOutput::getInstance()->removeCallback(0);
}

void AppDelegate::applicationWillEnterForeground()
{
    Director::getInstance()->startAnimation();
    AudioOutput::getInstance()->registerCallback([] (float *outbuf, int nframes) {
        for (int i = 0; i < nframes * 2; i++)
            outbuf[i] += sin((phase + i / 2) * M_PI / 100) * 0.2;
    });
}
