#include "MusicianNode.h"
#include "Global.h"
#include "data/GameData.h"
using namespace cocos2d;

bool MusicianNode::init()
{
    _drawNode = DrawNode::create();
    this->addChild(_drawNode);

    this->scheduleUpdate();
    return true;
}

void MusicianNode::clearAndRedrawBackground()
{
    Size size = this->getContentSize();

    _drawNode->clear();
    _drawNode->drawSolidRect(
        Vec2(0, 0),
        Vec2(size.width, size.height),
        Color4F(0.8, 0.8, 0.8, 0.8));
    _drawNode->drawSegment(
        Vec2(0, HIT_LINE_POS),
        Vec2(size.width, HIT_LINE_POS),
        2, Color4F(1, 1, 0.7, 0.9));
}

void MusicianNode::setContentSize(const Size &size)
{
    Node::setContentSize(size);
    _drawNode->setContentSize(size);
    this->clearAndRedrawBackground();
}

void MusicianNode::startPlay()
{
    _musician->startPlay();
    auto listener = EventListenerKeyboard::create();
    listener->onKeyPressed = [this](EventKeyboard::KeyCode key, Event *event) {
        _musician->sendEvent((int)key);
    };
    this->getEventDispatcher()
        ->addEventListenerWithSceneGraphPriority(listener, this);
}

void MusicianNode::update(float dt)
{
    _musician->tick(dt);
    uint32_t timePos = _musician->getTimePositionInTrack();

    Size size = this->getContentSize();
    this->clearAndRedrawBackground();
    _musician->draw(_drawNode);
}
