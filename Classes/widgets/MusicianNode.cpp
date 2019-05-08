#include "MusicianNode.h"
#include "Global.h"
#include "data/GameData.h"
using namespace cocos2d;

#define HIT_LINE_POS    0.333333

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
        Vec2(0, size.height * HIT_LINE_POS),
        Vec2(size.width, size.height * HIT_LINE_POS),
        2, Color4F(1, 1, 0.7, 0.9));
}

void MusicianNode::setContentSize(const Size &size)
{
    Node::setContentSize(size);
    this->clearAndRedrawBackground();
}

void MusicianNode::startPlay()
{
    _musician->startPlay();
    auto listener = EventListenerKeyboard::create();
    listener->onKeyPressed = [this](EventKeyboard::KeyCode key, Event *event) {
        printf("%d\n", (int)key);
    };
    this->getEventDispatcher()
        ->addEventListenerWithSceneGraphPriority(listener, this);
}

void MusicianNodeBasicKeys::update(float dt)
{
    _musician->tick(dt);
    uint32_t timePos = _musician->getTimePositionInTrack();

    std::vector<KeyNote *> notes;
    _musician->getNotesToDisplay(timePos, notes);

    Size size = this->getContentSize();
    this->clearAndRedrawBackground();

    for (auto n : notes) {
        auto nn = (KeyTrackBasicKeys<4>::Note *)n;
        float posX = size.width / 4 * nn->trackIdx;
        float posY = size.height * (HIT_LINE_POS + 0.0025 * (nn->time - timePos));
        _drawNode->drawSegment(
            Vec2(posX, posY),
            Vec2(posX + size.width / 4, posY),
            2, Color4F(0.5, 0.6, 1, 0.9));
    }
}
