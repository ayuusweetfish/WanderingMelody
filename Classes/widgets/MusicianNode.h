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

    bool init() override;
    CREATE_FUNC(MusicianNode);

    void clearAndRedrawBackground();
    void setContentSize(const cocos2d::Size &size) override;

    inline void setMusician(Musician *musician) { _musician = musician; }

    void startPlay();
    void update(float dt) override;

protected:
    cocos2d::DrawNode *_drawNode;
    Musician *_musician;
};

#endif
