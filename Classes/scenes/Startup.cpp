#include "Startup.h"
#include "Global.h"
#include "scenes/Select.h"
#include "scenes/Options.h"
using namespace cocos2d;

bool Startup::init()
{
    if (!LayerColor::initWithColor(Color4B(255, 255, 255, 255))) return false;

    auto label = Label::createWithTTF(
        "Hello wandering world!",
        "OpenSans-Light.ttf", 28);
    label->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    label->setPosition(WIN_SIZE / 2);
    label->setColor(Color3B(0, 0, 0));
    this->addChild(label);

    // Fade in on startup
    auto cover = LayerColor::create(Color4B::BLACK);
    this->addChild(cover, INT32_MAX);
    cover->runAction(FadeOut::create(0.15f));

    auto listener = cocos2d::EventListenerKeyboard::create();
    listener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event *event) {
        if (keyCode == EventKeyboard::KeyCode::KEY_ENTER) {
            auto scene = Select::createScene();
            GO_TO_SCENE(scene);
        } else if (keyCode == EventKeyboard::KeyCode::KEY_TAB) {
            auto scene = Options::createScene();
            GO_TO_SCENE(scene);
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}
