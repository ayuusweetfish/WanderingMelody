#include "Options.h"
#include "Global.h"
using namespace cocos2d;

bool Options::init()
{
    if (!LayerColor::initWithColor(Color4B(255, 255, 255, 255))) return false;

    auto label = Label::createWithTTF(
        "Hello options!",
        "OpenSans-Light.ttf", 28);
    label->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    label->setPosition(WIN_SIZE / 2);
    label->setColor(Color3B(0, 0, 0));
    this->addChild(label);

    auto listener = cocos2d::EventListenerKeyboard::create();
    listener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event *event) {
        if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) {
            GO_BACK_SCENE();
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
};
