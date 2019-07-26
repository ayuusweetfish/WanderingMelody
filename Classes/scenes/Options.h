#ifndef __WanderingMelody__Options_h__
#define __WanderingMelody__Options_h__

#include "cocos2d.h"
#include "Global.h"

class Options : public cocos2d::LayerColor
{
public:
    bool init() override;
    CREATE_FUNC(Options);
    SCENE_FUNC(Options);
};

#endif
