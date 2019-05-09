#ifndef __WanderingMelody__Gameplay_h__
#define __WanderingMelody__Gameplay_h__

#include "cocos2d.h"

#include "Global.h"
#include "gig/Gig.h"

class Gameplay : public cocos2d::LayerColor
{
public:
    bool init() override;
    CREATE_FUNC(Gameplay);
    SCENE_FUNC(Gameplay);

protected:
    Gig _gig;
};

#endif
