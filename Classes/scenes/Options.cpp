#include "Options.h"
#include "Global.h"
#include "widgets/ListMenu.h"
using namespace cocos2d;

#include <vector>

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

    std::vector<ListMenu::Item> items;
    for (int i = 0; i < 10; i++)
        items.push_back(ListMenu::Item("Hi", [] (ListMenu::Item &i) { },
            -i, -i * 2, 5, false));
    for (int i = 0; i < 10; i++)
        items.push_back(ListMenu::Item("Heya", [] (ListMenu::Item &i) { i._value = 0; },
            i % 3, {"Yes", "Yeah", "Yup"}));
    auto menu = ListMenu::createWithItems(items);
    menu->setContentSize(Size(WIN_W * 0.8, WIN_H * 0.75));
    menu->setPosition(Vec2(WIN_W * 0.1, WIN_H * 0.85));
    this->addChild(menu);

    auto listener = cocos2d::EventListenerKeyboard::create();
    listener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event *event) {
        if (Config::isKeyCancel(keyCode)) GO_BACK_SCENE();
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
};
