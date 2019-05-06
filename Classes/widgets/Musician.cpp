#include "Musician.h"
#include "Global.h"
using namespace cocos2d;

#define HIT_LINE_POS    0.25

bool Musician::init()
{
    _drawNode = DrawNode::create();
    this->addChild(_drawNode);

    this->scheduleUpdate();
    return true;
}

void Musician::setContentSize(const Size &size)
{
    Node::setContentSize(size);
    _drawNode->clear();
    _drawNode->drawSolidRect(
        Vec2(0, 0),
        Vec2(size.width, size.height),
        Color4F(0.8, 0.8, 0.8, 0.8));
    _drawNode->drawSegment(
        Vec2(0, size.height * HIT_LINE_POS),
        Vec2(size.width, size.height * HIT_LINE_POS),
        2, Color4F(1, 1, 0.7, 0.9));
}

void MusicianBasicKeys::update(float dt)
{
}
