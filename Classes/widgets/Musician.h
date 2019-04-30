#ifndef __WanderingMelody__Musician_h__
#define __WanderingMelody__Musician_h__

#include "cocos2d.h"

class Musician : public cocos2d::Node
{
public:
    virtual bool init() override;
    CREATE_FUNC(Musician);

    virtual void update(float dt) override;

protected:
};

#endif
