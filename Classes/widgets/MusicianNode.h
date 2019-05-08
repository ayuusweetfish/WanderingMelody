#ifndef __WanderingMelody__MusicianNode_h__
#define __WanderingMelody__MusicianNode_h__

#include "cocos2d.h"

class Musician;

class MusicianNode : public cocos2d::Node
{
public:
    MusicianNode() :
        _drawNode(nullptr),
        _musician(nullptr)
    { }

    virtual bool init() override;

    void clearAndRedrawBackground();
    virtual void setContentSize(const cocos2d::Size &size) override;

    inline void setMusician(Musician *musician) { _musician = musician; }

    void startPlay();
    virtual void update(float dt) override = 0;

protected:
    cocos2d::DrawNode *_drawNode;
    Musician *_musician;
};

class MusicianNodeBasicKeys : public MusicianNode
{
public:
    CREATE_FUNC(MusicianNodeBasicKeys);

    virtual void update(float dt) override;
};

#endif
