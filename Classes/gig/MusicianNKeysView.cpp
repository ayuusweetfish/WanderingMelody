#include "gig/MusicianNKeys.h"

#include "cocos2d.h"
#include "Global.h"
using namespace cocos2d;

template <int N> bool MusicianNKeys<N>::Display::init(MusicianNKeys<N> *mus)
{
    _mus = mus;

    _drawNode = DrawNode::create();
    if (!_drawNode) return false;
    this->addChild(_drawNode);

    auto listener = cocos2d::EventListenerKeyboard::create();
    listener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event *event) {
        _mus->sendEvent((int)keyCode);
    };
    listener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event *event) {
        _mus->sendEvent((int)keyCode + 1024);
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    this->scheduleUpdate();

    return true;
}

template <int N> void MusicianNKeys<N>::Display::update(float dt)
{
    _mus->tick(dt);
    _drawNode->clear();

    Size size = this->getContentSize();

    _drawNode->drawSolidRect(
        Vec2(0, 0),
        Vec2(size.width, size.height),
        Color4F(0.8, 0.8, 0.8, 0.8));
    _drawNode->drawSegment(
        Vec2(0, HIT_LINE_POS),
        Vec2(size.width, HIT_LINE_POS),
        2, Color4F(1, 1, 0.7, 0.9));

    for (const auto &n : _mus->_keyNotes) {
        float posX = size.width / 4 * n.track;
        float posY = HIT_LINE_POS + size.height * 0.0025 * (n.time - _mus->getCurTick());
        _drawNode->drawSegment(
            Vec2(posX, posY),
            Vec2(posX + size.width / 4, posY),
            2,
            n.triggered == -1 ? Color4F(0.5, 0.6, 1, 0.9) : Color4F(1, 0.8, 0.4, 0.9));
    }
}
