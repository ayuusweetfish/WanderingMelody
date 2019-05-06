#ifndef __WanderingMelody__MusicianNode_h__
#define __WanderingMelody__MusicianNode_h__

#include "cocos2d.h"

class MusicianNode : public cocos2d::Node
{
public:
    virtual bool init() override;

    virtual void setContentSize(const cocos2d::Size &size) override;
    virtual void update(float dt) override = 0;

protected:
    cocos2d::DrawNode *_drawNode;
};

class MusicianNodeBasicKeys : public MusicianNode
{
public:
    CREATE_FUNC(MusicianNodeBasicKeys);

    virtual void update(float dt) override;
};

#endif
