#include "Startup.h"
#include "Global.h"
#include "scenes/Select.h"
#include "scenes/Options.h"
#include "widgets/ListMenu.h"
using namespace cocos2d;

bool Startup::init()
{
    if (!LayerColor::initWithColor(Color4B(255, 255, 255, 255))) return false;

    auto label = Label::createWithTTF(
        "Wandering Melody",
        "OpenSans-Light.ttf", 64);
    label->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    label->setPosition(Vec2(WIN_W / 2, WIN_H * 0.7));
    label->setColor(Color3B(0, 0, 0));
    this->addChild(label);

    // Fade in on startup
    auto cover = LayerColor::create(Color4B::BLACK);
    this->addChild(cover, INT32_MAX);
    cover->runAction(FadeOut::create(0.15f));

    auto menu = new ListMenu();
    std::vector<ListMenu::Item> items;

    items.push_back(ListMenu::Item("Wander", [this] (ListMenu::Item &) {
        auto scene = Select::createScene();
        GO_TO_SCENE(scene);
    }));

    items.push_back(ListMenu::Item("Options", [this] (ListMenu::Item &) {
        auto scene = Options::createScene();
        GO_TO_SCENE(scene);
    }));

    items.push_back(ListMenu::Item("Quit", [this] (ListMenu::Item &) {
        Director::getInstance()->end();
    }));

    menu->initWithItems(items, 36, true);
    menu->setContentSize(Size(WIN_W * 0.2, WIN_H * 0.5));
    menu->setPosition(Vec2(WIN_W * 0.4, WIN_H * 0.5));
    this->addChild(menu);

    return true;
}
