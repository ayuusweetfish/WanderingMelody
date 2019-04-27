#include "Startup.h"
#include "Global.h"
using namespace cocos2d;

bool Startup::init()
{
    if (!LayerColor::initWithColor(Color4B(255, 255, 255, 255))) return false;

    auto vsize = Director::getInstance()->getVisibleSize();

    auto label = Label::createWithSystemFont(
        "Hello wandering world!", "Arial", 42);
    label->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    label->setPosition(vsize / 2);
    label->setColor(Color3B(0, 0, 0));
    this->addChild(label);

    // Fade in on startup
    auto cover = LayerColor::create(Color4B::BLACK);
    this->addChild(cover, INT32_MAX);
    cover->runAction(FadeOut::create(0.15f));

    return true;
}
