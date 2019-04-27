#ifndef __WanderingMelody__AppDelegate_h__
#define __WanderingMelody__AppDelegate_h__

#include "cocos2d.h"

class AppDelegate : private cocos2d::Application
{
public:
    virtual bool applicationDidFinishLaunching();
    virtual void applicationDidEnterBackground();
    virtual void applicationWillEnterForeground();
};

#endif
