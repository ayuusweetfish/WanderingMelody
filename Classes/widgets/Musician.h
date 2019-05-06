#ifndef __WanderingMelody__Musician_h__
#define __WanderingMelody__Musician_h__

#include "cocos2d.h"

class Musician : public cocos2d::Node
{
public:
    virtual bool init() override;

    virtual void setContentSize(const cocos2d::Size &size) override;
    virtual void update(float dt) override = 0;

protected:
    cocos2d::DrawNode *_drawNode;
};

class MusicianBasicKeys : public Musician
{
public:
    CREATE_FUNC(MusicianBasicKeys);

    virtual void update(float dt) override;
};

#endif
