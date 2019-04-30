#ifndef __WanderingMelody__Startup_h__
#define __WanderingMelody__Startup_h__

#include "cocos2d.h"
#include "Global.h"

class Startup : public cocos2d::LayerColor
{
public:
    bool init() override;
    CREATE_FUNC(Startup);
    SCENE_FUNC(Startup);
};

#endif
